// Copyright 2014 Samsung Electronics Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/browser/media/tizen/webaudio_decoder_browser_gstreamer.h"

#include "base/bind.h"
#include "base/strings/string_util.h"
#include "base/time/time.h"
#include "gst/app/gstappsink.h"
#include "gst/audio/audio.h"
#include "media/base/audio_bus.h"
#include "media/base/limits.h"
#include "media/base/tizen/webaudio_media_codec_info_tizen.h"
#include "third_party/WebKit/public/platform/WebAudioBus.h"

namespace content {

#define CHUNK_SIZE 204800  // (4096*50)

static int gst_dec_count_ = 32;

////////////////////////////////////////
// GSTDecoder class - declaration
class GSTDecoder {
 public:
  void InitializeGstDestination(int pcm_output,
                                uint16_t number_of_channels,
                                uint32_t sample_rate,
                                size_t number_of_frames);
  void SendGstOutputUsinghandle(int pcm_output, uint8_t* buffer, int buf_size);
  static void MediaFileDecoder(GstAppData* appData);

  // callbacks
  static void cb_newpad (GstElement* decodebin, GstPad* pad, GstAppData* data);
  static void cb_need_data (GstElement* source, guint size, GstAppData* data);
  static void cb_eos (GstAppSink* sink, gpointer user_data);
  static GstFlowReturn cb_new_preroll (GstAppSink* sink, gpointer user_data);
  static GstFlowReturn cb_new_buffer (GstAppSink* sink, gpointer user_data);
  static GstBusSyncReply cb_pipeline_message (GstBus* bus,
                                              GstMessage* message,
                                              GstAppData* data);
};  // GSTDecoder class

void GSTDecoder::InitializeGstDestination(int pcm_output,
                                          uint16_t number_of_channels,
                                          uint32_t sample_rate,
                                          size_t number_of_frames) {
  struct media::WebAudioMediaCodecInfoTizen info = {
      static_cast<unsigned long>(number_of_channels),
      static_cast<unsigned long>(sample_rate),
      static_cast<unsigned long>(number_of_frames)
  };

  HANDLE_EINTR(write(pcm_output, &info, sizeof(info)));
}

void GSTDecoder::SendGstOutputUsinghandle(int pcm_output, uint8_t * buffer, int buf_size) {
  size_t count = buf_size;
  ssize_t total_bytes = 0;
  while (count > 0) {
    int bytes_to_write = (count >= PIPE_BUF) ? PIPE_BUF : count;
    ssize_t bytes_written = HANDLE_EINTR(write(pcm_output, buffer, bytes_to_write));
    if (bytes_written == -1)
      break;
    count -= bytes_written;
    buffer += bytes_written;
    total_bytes += bytes_written;
  }
  return;
}

void GSTDecoder::MediaFileDecoder(GstAppData* appData) {
  if(!appData)
    return;

  if (!gst_is_initialized()) {
    GError* err = NULL;
    if (!gst_init_check(NULL, NULL, &err)) {
      LOG(ERROR) << "Gst could not be initialized";
      close(appData->pcm_output_);
      return;
    }
  }

  // makes gst-element(s)
  appData->app_src_ = gst_element_factory_make("appsrc", NULL);
  appData->app_sink_ = gst_element_factory_make("appsink", NULL);
  appData->decoder_ = gst_element_factory_make("decodebin2", NULL);
  appData->convert_ = gst_element_factory_make("audioconvert", NULL);

  appData->resample_ = gst_element_factory_make ("audioresample", NULL);
  appData->capsfilter_ = gst_element_factory_make("capsfilter", NULL);
  appData->caps_ = gst_caps_new_simple("audio/x-raw-int", "width", G_TYPE_INT, 16, NULL);

  if (!appData->app_src_ || !appData->app_sink_ || !appData->decoder_ ||
      !appData->convert_ || !appData->resample_ || !appData->capsfilter_ ||
      !appData->caps_) {
    LOG(ERROR) << "Creation of one or more gst-element(s) for decoder pipeline failed";
    return;
  }

  g_object_set(G_OBJECT(appData->capsfilter_), "caps", appData->caps_, NULL);
  gst_caps_unref(appData->caps_);

  // sets propeties for element(s)
  g_object_set(G_OBJECT(appData->app_sink_), "sync", FALSE, NULL);

  // connects signal(s) to element(s)
  g_signal_connect(appData->app_src_, "need-data", G_CALLBACK(cb_need_data), appData);
  g_signal_connect(appData->decoder_, "pad-added", G_CALLBACK(cb_newpad), appData);

  GstAppSinkCallbacks callbacks = { cb_eos, cb_new_preroll,
                                    cb_new_buffer, NULL,
                                    { NULL, NULL, NULL } };
  gst_app_sink_set_callbacks(GST_APP_SINK(appData->app_sink_), &callbacks, appData, NULL);

  //FIXME: gst-element(s) can have 94 each name, but this can not be enough
  gchar pipeline_name[16] = {0,};
  sprintf(pipeline_name, "pipeln_%d", gst_dec_count_);

  memset(appData->audioout_name_, 0, 16);
  sprintf(appData->audioout_name_, "audout_%d", gst_dec_count_);

  gst_dec_count_++;
  if (gst_dec_count_ > 126)
    gst_dec_count_ = 32;

  // makes gst-pipeline
  appData->pipeline_ = gst_pipeline_new((const gchar*)&pipeline_name);
  gst_bin_add_many(GST_BIN(appData->pipeline_), appData->app_src_, appData->decoder_, NULL);
  if (!gst_element_link(appData->app_src_, appData->decoder_)) {
    LOG(ERROR) << __FUNCTION__ << " Something wrong on gst initialization";
    if ( appData->pipeline_ )
      gst_object_unref(appData->pipeline_);
    return;
  }

  // makes audio-output and links it to gst-pipeline
  appData->audioout_ = gst_bin_new(appData->audioout_name_);
  appData->audiopad_ = gst_element_get_static_pad(appData->convert_, "sink");

  gst_bin_add_many(GST_BIN(appData->audioout_), appData->convert_,
      appData->resample_, appData->capsfilter_, appData->app_sink_, NULL);
  if (!gst_element_link_many(appData->convert_, appData->resample_,
      appData->capsfilter_, appData->app_sink_, NULL)) {
    LOG(ERROR) << __FUNCTION__ << "Some element could not be linked";
    if ( appData->pipeline_ )
      gst_object_unref(appData->pipeline_);
    return;
  }

  gst_element_add_pad(appData->audioout_, gst_ghost_pad_new("sink", appData->audiopad_));
  gst_object_unref(appData->audiopad_);
  gst_bin_add(GST_BIN(appData->pipeline_), appData->audioout_);

  GstBus* bus = gst_pipeline_get_bus(GST_PIPELINE(appData->pipeline_));
  if (!bus) {
    LOG(ERROR) << __FUNCTION__ << "GStreamer bus creation failed";
    if ( appData->pipeline_ )
      gst_object_unref(appData->pipeline_);
    return;
  }
  gst_bus_set_sync_handler(bus, (GstBusSyncHandler)cb_pipeline_message, appData);

  // actually works decoding
  gst_element_set_state(appData->pipeline_, GST_STATE_PLAYING);

  //FIXME: Check if its possible to remove usleep() and make any gst
  //async call so that GST wont block on Browser UI thread.
  while (appData->isRunning_) {
    usleep(10);
  }

  // returns resource(s)
  g_signal_handlers_disconnect_by_func(
      bus, reinterpret_cast<gpointer>(cb_pipeline_message), appData);
  gst_bus_set_sync_handler(bus, NULL, NULL);
  gst_object_unref(bus);
  gst_element_set_state(appData->pipeline_, GST_STATE_NULL);
  gst_object_unref(appData->pipeline_);
}

void GSTDecoder::cb_newpad(GstElement* decodebin, GstPad* pad, GstAppData* data) {
  GstPad* sink_pad = gst_element_get_pad(data->audioout_, "sink");
  if (GST_PAD_IS_LINKED(sink_pad)) {
    g_object_unref(sink_pad);
    return;
  }

  GstCaps* caps = NULL;
  GstStructure* str = NULL;

  caps = gst_pad_get_caps(pad);
  if (caps) {
    str = gst_caps_get_structure(caps, 0);
    if (str) {
      if (!g_strrstr(gst_structure_get_name(str), data->audioout_name_))
        gst_pad_link(pad, sink_pad);
    }
  }
  g_object_unref(sink_pad);
  gst_caps_unref(caps);
}

void GSTDecoder::cb_need_data (GstElement* source, guint size, GstAppData* data) {
  if (data->isEndOfStream_)
    return;

  GstBuffer* buffer = gst_buffer_new();
  if (!buffer)
    return;

  guint len = CHUNK_SIZE;
  if ((data->enc_offset_ + len ) > data->enc_length_)
    len = data->enc_length_ - data->enc_offset_;

  GST_BUFFER_DATA(buffer) = data->encodeddata_ + data->enc_offset_;
  GST_BUFFER_SIZE(buffer) = len;
  data->enc_offset_ += len;

  GstFlowReturn ret = GST_FLOW_OK;
  g_signal_emit_by_name(data->app_src_, "push-buffer", buffer, &ret);

  if (ret != GST_FLOW_OK) {
    LOG(ERROR) << "cb_need_data: push-buffer ret: FAILED";
    data->isRunning_ = false;
  }

  if (data->enc_offset_ >= data->enc_length_) {
    data->isEndOfStream_ = TRUE;
    g_signal_emit_by_name(data->app_src_, "end-of-stream", &ret);
  }

  gst_buffer_unref(buffer);
}

void GSTDecoder::cb_eos (GstAppSink* sink, gpointer user_data) {
  GstAppData* data = reinterpret_cast<GstAppData*>(user_data);
  if (!data->isEndOfStream_) {
    LOG(ERROR) << "not end of stream yet appsrc-side";
  }

  close(data->pcm_output_);
  data->isRunning_ = false;
}

GstFlowReturn GSTDecoder::cb_new_preroll (GstAppSink* sink, gpointer user_data) {
  return GST_FLOW_OK;
}

GstFlowReturn GSTDecoder::cb_new_buffer (GstAppSink* sink, gpointer user_data) {
  GstBuffer* buffer = gst_app_sink_pull_buffer(GST_APP_SINK(sink));

  if (!buffer)
    return GST_FLOW_ERROR;

  GstAppData* data = (GstAppData*) (user_data);

  if (data->isNewRequest_) {
    GstCaps* caps = NULL;
    GstStructure* str = NULL;
    caps = gst_buffer_get_caps(buffer);
    if (caps)
      str = gst_caps_get_structure(caps, 0);

    gboolean ret = true;
    gint channel = 0;
    gint rate = 0;
    gint width = 0;
    if (caps && str) {
      ret &= gst_structure_get_int(str, "channels", &channel);
      ret &= gst_structure_get_int(str, "rate", &rate);
      ret &= gst_structure_get_int(str, "width", &width);
    }

    if (!caps || !str || !ret || !channel || !rate || !width) {
      gst_caps_unref(caps);
      gst_buffer_unref(buffer);
      return GST_FLOW_ERROR;
    }

    GstClockTime duration = (static_cast<guint64> (GST_BUFFER_SIZE(buffer))*8*GST_SECOND)
                                / (channel*rate*width);
    int frames = GST_CLOCK_TIME_TO_FRAMES(duration, rate);

    data->gst_decoder_->InitializeGstDestination(data->pcm_output_, channel, rate, frames);
    data->isNewRequest_ = false;
  }

  if (buffer && buffer->size > 0) {
    data->gst_decoder_->SendGstOutputUsinghandle(data->pcm_output_,
                                                 buffer->data,
                                                 buffer->size);
  }

  gst_buffer_unref(buffer);
  return GST_FLOW_OK;
}

GstBusSyncReply GSTDecoder::cb_pipeline_message(GstBus* bus,
                                                GstMessage* message,
                                                GstAppData* data) {
  switch (GST_MESSAGE_TYPE(message)) {
    case GST_MESSAGE_ERROR:
      GError* error;
      gst_message_parse_error(message, &error, NULL);
      LOG(ERROR) << "Error message : " << error->message
          << " recieved from : "<< GST_MESSAGE_SRC_NAME(message)
          << ", error code : " << error->code;
      g_error_free(error);

      if (data->isRunning_) {
        close(data->pcm_output_);
        data->isRunning_ = false;
      }
      break;
    default:
      LOG(WARNING) << "Unhandled GStreamer message type : "
          << GST_MESSAGE_TYPE_NAME(message);
      break;
  }

  gst_message_unref(message);

  return GST_BUS_DROP;
}

////////////////////////////////////////
// BrowserMessageFilterTizen class

// static
BrowserMessageFilterTizen* BrowserMessageFilterTizen::GetInstance() {
  return Singleton<BrowserMessageFilterTizen>::get();
}

BrowserMessageFilterTizen::BrowserMessageFilterTizen()
    : gst_thread_("GstThread") {
}

BrowserMessageFilterTizen::~BrowserMessageFilterTizen() {
}

void BrowserMessageFilterTizen::DecodeUsingGST(
    base::SharedMemoryHandle foreign_memory_handle,
    base::FileDescriptor pcm_output,
    uint32_t data_size) {

  base::SharedMemory shared_memory(foreign_memory_handle, false);
  shared_memory.Map(data_size);

  GstAppData* data = new GstAppData;
  if (!data)
    return;

  memset(data, 0, sizeof(GstAppData));

  data->encodeddata_ = (uint8_t*) (malloc(data_size));
  if (!data->encodeddata_) {
    LOG(ERROR) << "BrowserMessageFilterTizen::"<<__FUNCTION__
        << " - encodeddata malloc failed";
    delete data;
    return;
  }
  memcpy(data->encodeddata_, static_cast<uint8_t*>(shared_memory.memory()), data_size);

  data->isRunning_ = true;
  data->enc_length_ = data_size;
  data->pcm_output_ = pcm_output.fd;
  data->isEndOfStream_ = false;
  data->isNewRequest_ = true;

  GSTDecoder *decoder = new GSTDecoder();
  if (decoder) {
    data->gst_decoder_ = decoder;
    decoder->MediaFileDecoder(data);
    delete decoder;
  }

  if (data->encodeddata_) {
    free(data->encodeddata_);
    data->encodeddata_ = NULL;
  }
  delete data;
}

void BrowserMessageFilterTizen::EncodedDataReceived(
    base::SharedMemoryHandle foreign_memory_handle,
    base::FileDescriptor pcm_output,
    uint32_t data_size) {

  if (!gst_thread_.IsRunning() && !gst_thread_.Start()) {
    LOG(ERROR) << "BrowserMessageFilterTizen::"<<__FUNCTION__
        << " - Starting GStreamer thread failed";
    return;
  }

  gst_thread_.message_loop()->PostTask(FROM_HERE,
      base::Bind(&BrowserMessageFilterTizen::DecodeUsingGST,
      base::Unretained(this), foreign_memory_handle,
      pcm_output, data_size));
}

}  // namespace content
