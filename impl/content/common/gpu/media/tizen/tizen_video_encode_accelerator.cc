// Copyright 2014 Samsung Electronics Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/common/gpu/media/tizen/tizen_video_encode_accelerator.h"

#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <gst/app/gstappsrc.h>
#include <gst/video/video.h>

#include "base/bind.h"
#include "base/command_line.h"
#include "base/logging.h"
#include "base/message_loop/message_loop.h"
#include "base/timer/timer.h"
#include "gpu/command_buffer/service/gpu_switches.h"
#include "media/base/bitstream_buffer.h"
#include "third_party/webrtc/common_video/libyuv/include/webrtc_libyuv.h"

using media::VideoFrame;

namespace content {

struct GstBufferDeleter {
  inline void operator()(GstBuffer* ptr) const {
    DCHECK(ptr != NULL);
    gst_buffer_unref(ptr);
  }
};

struct GstElementDeleter {
  inline void operator()(GstElement* ptr) const {
    DCHECK(ptr != NULL);
    gst_object_unref(ptr);
  }
};

enum {
  // Arbitrary choice.
  INITIAL_FRAMERATE = 30,
  // Until there are non-realtime users, no need for unrequested I-frames.
  INPUT_BUFFER_COUNT = 5, // default input buffer counts of omx_h264enc
  MAX_BUFFERING = 60,
  MAX_FRAME_RATE = 30,
  // Max bitrate in bps
  MAX_BITRATE = 2000000
};

media::VideoEncodeAccelerator* CreateTizenVideoEncodeAccelerator() {
  return new TizenVideoEncodeAccelerator();
}

struct TizenVideoEncodeAccelerator::BitstreamBufferRef {
  BitstreamBufferRef(
      const scoped_refptr<media::VideoFrame>& frame,
      base::WeakPtr<media::VideoEncodeAccelerator::Client> client_delegate,
      const scoped_refptr<base::MessageLoopProxy>& client_message_loop_proxy,
      size_t size)
      : frame_(frame),
        client_delegate_(client_delegate),
        client_message_loop_proxy_(client_message_loop_proxy),
        size_(size),
        bytes_used_(0),
        gst_buffer_(NULL) {}

  ~BitstreamBufferRef() {}

  static void Destruct(gpointer data) {
    DCHECK(data != NULL);
    BitstreamBufferRef* pRef = static_cast<BitstreamBufferRef*>(data);
    delete pRef;
  }

  scoped_refptr<media::VideoFrame> frame_;
  base::WeakPtr<media::VideoEncodeAccelerator::Client> client_delegate_;
  scoped_refptr<base::MessageLoopProxy> client_message_loop_proxy_;
  size_t size_;
  off_t bytes_used_;
  GstBuffer* gst_buffer_;
};

struct TizenVideoEncodeAccelerator::Impl {
  Impl(media::VideoEncodeAccelerator::Client* client,
       scoped_refptr<base::MessageLoopProxy> msg_loop)
      : pipeline_(NULL),
        encoder_(NULL),
        appsrc_(NULL),
        gst_thread_("GSTEncoder"),
        enable_framedrop_(false),
        io_client_weak_factory_(client),
        child_message_loop_proxy_(msg_loop),
        gst_bitrate_(0),
        is_running_(false),
        is_destroying_(false),
        can_feed_(false) {}

  void DeliverVideoFrame(GstBuffer* buffer, bool key_frame);
  static GstFlowReturn OnEncoded(GstAppSink *sink,gpointer data);
  static void StartFeed(GstAppSrc *source, guint size, gpointer data);
  static void StopFeed(GstAppSrc *source, gpointer data);

  GstElement* pipeline_;
  GstElement* encoder_;
  GstElement* appsrc_;
  base::Thread gst_thread_;
  bool enable_framedrop_;
  std::vector<media::BitstreamBuffer> encoder_output_queue_;
  base::WeakPtrFactory<media::VideoEncodeAccelerator::Client> io_client_weak_factory_;
  const scoped_refptr<base::MessageLoopProxy> child_message_loop_proxy_;
  base::Lock destroy_lock_;
  base::ThreadChecker thread_checker_;
  gfx::Size view_size_;
  uint32 gst_bitrate_;
  volatile bool is_running_;
  volatile bool is_destroying_;
  volatile bool can_feed_;
};

void TizenVideoEncodeAccelerator::Impl::DeliverVideoFrame(GstBuffer* buffer,
                                                          bool key_frame) {
  media::BitstreamBuffer* bs_buffer = NULL;
  scoped_ptr<base::SharedMemory> shm;
  scoped_ptr<GstBuffer, GstBufferDeleter> gst_buffer(buffer);

  if (gst_buffer == NULL) {
    LOG(ERROR) << __FUNCTION__ << " Invalid buffer for delivering";
    return;
  }

  if (enable_framedrop_ && key_frame) {
    enable_framedrop_ = false;
  }

  if (enable_framedrop_) {
    DVLOG(2) << "OUTPUT QUEUE IS EMPTY !!!!!!!! DELTA FRAME DROP.";
    return;
  }

  if (encoder_output_queue_.empty()) {
    enable_framedrop_ = true;
    DVLOG(2) << "OUTPUT QUEUE IS EMPTY !!!!!!!! FRAME DROP.";
    return;
  }

  bs_buffer = &encoder_output_queue_.back();
  encoder_output_queue_.pop_back();

  DVLOG(3) << __FUNCTION__
           << " output buffer in use : buffer_id: " << bs_buffer->id()
           << " queue size: " << encoder_output_queue_.size();

  shm.reset(new base::SharedMemory(bs_buffer->handle(), false));
  if (!shm->Map(bs_buffer->size())) {
    LOG(ERROR) << "Failed to map SHM";
    return;
  }
  if (gst_buffer->size > shm->mapped_size()) {
    LOG(ERROR) << "Encoded buff too large: "
               << gst_buffer->size << ">" << shm->mapped_size();
    return;
  }

  //copying data to shared memory.
  memcpy(static_cast<uint8*>(shm->memory()), gst_buffer->data, gst_buffer->size);

  child_message_loop_proxy_->PostTask(
      FROM_HERE,
      base::Bind(&media::VideoEncodeAccelerator::Client::BitstreamBufferReady,
                 io_client_weak_factory_.GetWeakPtr(),
                 bs_buffer->id(),
                 gst_buffer->size,
                 key_frame));
}

GstFlowReturn TizenVideoEncodeAccelerator::Impl::OnEncoded(
    GstAppSink *sink, gpointer data) {
  bool key_frame = false;
  GstBuffer* gst_output_buf = NULL;
  TizenVideoEncodeAccelerator::Impl* impl =
      static_cast<TizenVideoEncodeAccelerator::Impl*>(data);

  gst_output_buf = gst_app_sink_pull_buffer(GST_APP_SINK(sink));

  if (gst_output_buf) {
    if (!GST_BUFFER_FLAG_IS_SET(gst_output_buf, GST_BUFFER_FLAG_DELTA_UNIT)) {
      key_frame = true;
    }
    if (gst_output_buf->data) {
      impl->gst_thread_.message_loop()->PostTask(
          FROM_HERE,
          base::Bind(&TizenVideoEncodeAccelerator::Impl::DeliverVideoFrame,
                     base::Unretained(impl),
                     gst_output_buf,
                     key_frame));
    }
  } else {
    LOG(ERROR) << __FUNCTION__ << " ENCODING FRAME FAILED";
  }

  return GST_FLOW_OK;
}

void TizenVideoEncodeAccelerator::Impl::StartFeed(
    GstAppSrc *source, guint size, gpointer data) {
  TizenVideoEncodeAccelerator::Impl* impl =
      static_cast<TizenVideoEncodeAccelerator::Impl*>(data);
  DCHECK(impl);
  impl->can_feed_ = true;
}

void TizenVideoEncodeAccelerator::Impl::StopFeed(
    GstAppSrc *source, gpointer data) {
  TizenVideoEncodeAccelerator::Impl* impl =
      static_cast<TizenVideoEncodeAccelerator::Impl*>(data);
  DCHECK(impl);
  impl->can_feed_ = false;
}

TizenVideoEncodeAccelerator::TizenVideoEncodeAccelerator()
    : impl_(NULL) {}

TizenVideoEncodeAccelerator::~TizenVideoEncodeAccelerator() {}

std::vector<media::VideoEncodeAccelerator::SupportedProfile>
TizenVideoEncodeAccelerator::GetSupportedProfiles() {
  std::vector<media::VideoEncodeAccelerator::SupportedProfile> profiles;
  media::VideoEncodeAccelerator::SupportedProfile profile;
  profile.profile = media::H264PROFILE_BASELINE;
  profile.max_resolution.SetSize(1280, 720);
  profile.max_framerate_numerator = 30;
  profile.max_framerate_denominator = 1;
  profiles.push_back(profile);

  return profiles;
}

bool TizenVideoEncodeAccelerator::Initialize(
    media::VideoFrame::Format input_format,
    const gfx::Size& input_visible_size,
    media::VideoCodecProfile output_profile,
    uint32 initial_bitrate,
    Client* client) {
  DVLOG(1) << __FUNCTION__
           << " size :" << input_visible_size.ToString()
           << " max bitrate :" << MAX_BITRATE << "bps";
  DCHECK(impl_ == NULL);
  if (media::H264PROFILE_MIN > output_profile ||
      media::H264PROFILE_MAX < output_profile) {
    NOTREACHED();
    return false;
  }

  impl_ = new Impl(client, base::MessageLoopProxy::current());
  impl_->gst_bitrate_ = initial_bitrate;
  impl_->view_size_ = input_visible_size;
  impl_->gst_thread_.Start();

  if (!StartEncoder()) {
    delete impl_;
    impl_ = NULL;
    return false;
  }

  base::MessageLoop::current()->PostTask(
      FROM_HERE,
      base::Bind(&media::VideoEncodeAccelerator::Client::RequireBitstreamBuffers,
                 impl_->io_client_weak_factory_.GetWeakPtr(),
                 (unsigned int)INPUT_BUFFER_COUNT,
                 input_visible_size,
                 MAX_BITRATE / 8)); // Maximum bytes for a frame by MAX_BITRATE.
  return true;
}

void TizenVideoEncodeAccelerator::Encode(
    const scoped_refptr<media::VideoFrame>& frame,
    bool force_keyframe) {
  size_t frame_size = VideoFrame::AllocationSize(VideoFrame::I420,
                                                 frame->coded_size());
  DVLOG(3) << __FUNCTION__
           << " coded_size :" << frame->coded_size().ToString()
           << " natural_size :" << frame->natural_size().ToString();

  scoped_ptr<BitstreamBufferRef> buffer_ref;

  buffer_ref.reset(
      new BitstreamBufferRef(frame,
                             impl_->io_client_weak_factory_.GetWeakPtr(),
                             impl_->child_message_loop_proxy_,
                             frame_size));

  if (!buffer_ref) {
    LOG(ERROR) << __FUNCTION__ << "malloc failed";
    return;
  }

  if (impl_->can_feed_ && !impl_->is_destroying_) {
    impl_->gst_thread_.message_loop()->PostTask(
        FROM_HERE,
        base::Bind(&TizenVideoEncodeAccelerator::OnEncode,
                   base::Unretained(this),
                   base::Passed(&buffer_ref),
                   force_keyframe));
  } else {
    DVLOG(2) << __FUNCTION__ << " [WEBRTC] . FRAME DROP :"
             << " can_feed_:" << impl_->can_feed_
             << " is_destroying_:" << impl_->is_destroying_;
  }
}

void TizenVideoEncodeAccelerator::UseOutputBitstreamBuffer(
    const media::BitstreamBuffer& buffer) {
  impl_->gst_thread_.message_loop()->PostTask(
      FROM_HERE,
      base::Bind(&TizenVideoEncodeAccelerator::OnUseOutputBitstreamBuffer,
                 base::Unretained(this),
                 buffer));
}

void TizenVideoEncodeAccelerator::RequestEncodingParametersChange(
    uint32 bitrate, uint32 framerate) {
  DVLOG(2) << __FUNCTION__
           << " bitrate: " << bitrate
           << " framerate: " << framerate;
  if (bitrate > 0 && bitrate != impl_->gst_bitrate_) {
    impl_->gst_bitrate_ = bitrate;
    // Omx Encoder expects bitrate in bps whereas ffenc expects bitrate in kbps
    // Information can be gained by gst-inspect
#if defined(OS_TIZEN)
    g_object_set(G_OBJECT(impl_->encoder_),
                 "bitrate",
                 std::min(bitrate, static_cast<uint32>(MAX_BITRATE)),
                 NULL);
#else
    g_object_set(G_OBJECT(impl_->encoder_),
                 "bitrate",
                 std::min(bitrate, static_cast<uint32>(MAX_BITRATE)) / 1000,
                 NULL);
#endif
  }
}

void TizenVideoEncodeAccelerator::Destroy() {
  if (impl_) {
    DCHECK(impl_->thread_checker_.CalledOnValidThread());
    if(impl_->appsrc_)
      gst_app_src_end_of_stream(GST_APP_SRC(impl_->appsrc_));

    {
      base::AutoLock auto_lock(impl_->destroy_lock_);
      impl_->is_destroying_ = true;
    }

    if (impl_->gst_thread_.IsRunning())
      impl_->gst_thread_.Stop();
    if (impl_->pipeline_) {
      gst_element_set_state(impl_->pipeline_, GST_STATE_NULL);
      gst_object_unref(GST_OBJECT(impl_->pipeline_));
    }

    DVLOG(1) << __FUNCTION__
             << " queue size: "<< impl_->encoder_output_queue_.size();

    while (!impl_->encoder_output_queue_.empty()) {
      media::BitstreamBuffer bitstream_buffer = impl_->encoder_output_queue_.back();
      // created shm and let it go out of scope automatically.
      scoped_ptr<base::SharedMemory> shm(
          new base::SharedMemory(bitstream_buffer.handle(), false));
      impl_->encoder_output_queue_.pop_back();
    }
    delete impl_;
  }
  delete this;
}

void TizenVideoEncodeAccelerator::OnEncode(
    scoped_ptr<BitstreamBufferRef> buffer_ref, bool force_keyframe) {

  BitstreamBufferRef* bufref = buffer_ref.release();

  if (bufref == NULL) {
    return;
  }

#if defined(OS_TIZEN)
  g_object_set(impl_->encoder_,
               "force-i-frame",
               (force_keyframe || impl_->enable_framedrop_) ? TRUE : FALSE,
               NULL);
#endif

  if (!(bufref->gst_buffer_ = gst_buffer_new())) {
    LOG(ERROR) << __FUNCTION__ << " malloc failed";
    return;
  }

  GST_BUFFER_MALLOCDATA(bufref->gst_buffer_) = reinterpret_cast<uint8*>(bufref);
  GST_BUFFER_FREE_FUNC(bufref->gst_buffer_) = BitstreamBufferRef::Destruct;
  GST_BUFFER_SIZE(bufref->gst_buffer_) = bufref->size_;
  GST_BUFFER_DATA(bufref->gst_buffer_) =
      static_cast<guint8*>(bufref->frame_->data(VideoFrame::kYPlane));

  if (GST_FLOW_OK !=
         gst_app_src_push_buffer(GST_APP_SRC(impl_->appsrc_),
                                 bufref->gst_buffer_)) {
    LOG(ERROR) << __FUNCTION__
               << " error while pushing buffer int appsrc on encoder.";
    gst_buffer_unref(bufref->gst_buffer_);
  }

  return;
}

void TizenVideoEncodeAccelerator::OnUseOutputBitstreamBuffer(
    const media::BitstreamBuffer& buffer) {
  impl_->encoder_output_queue_.push_back(buffer);

  DVLOG(2) << __FUNCTION__
           << " output buffer is ready to use: " << buffer.id()
           << " out queue size: " << impl_->encoder_output_queue_.size();
}

bool TizenVideoEncodeAccelerator::StartEncoder() {
  GError* error = NULL;
  GstCaps* appsrc_caps = NULL, *converter_caps = NULL;
  GstElement* gst_converter = NULL, *gst_appsink = NULL;
  gboolean retval = FALSE;
  scoped_ptr<GstElement, GstElementDeleter> gst_pipeline;

  guint64 max_input_buffer =
      INPUT_BUFFER_COUNT * VideoFrame::AllocationSize(VideoFrame::I420,
                                                      impl_->view_size_);
  GstAppSrcCallbacks appsrc_callbacks =
      { &TizenVideoEncodeAccelerator::Impl::StartFeed,
        &TizenVideoEncodeAccelerator::Impl::StopFeed,
        NULL };
  GstAppSinkCallbacks appsink_callbacks =
      { NULL, NULL, &TizenVideoEncodeAccelerator::Impl::OnEncoded, NULL };

  if (impl_->pipeline_ != NULL) {
    return false;
  }

  if (!gst_is_initialized() && !gst_init_check(NULL, NULL, &error)) {
    LOG(ERROR) << __FUNCTION__ << " cannot initialize gstreamer.";
    g_error_free(error);
    return false;
  }

  // pipeline initialization
  gst_pipeline.reset(gst_pipeline_new("h264_encode"));
  if (!gst_pipeline) {
    LOG(ERROR) << __FUNCTION__ << " cannot initialize gst pipeline.";
    return false;
  }

  // appsrc initialization
  if (!(impl_->appsrc_ = gst_element_factory_make ("appsrc", "src"))) {
    LOG(ERROR) << __FUNCTION__ << "cannot initialize gst appsrc.";
    return false;
  }
  if (!gst_bin_add(GST_BIN(gst_pipeline.get()), impl_->appsrc_)) {
    LOG(ERROR) << __FUNCTION__ << " cannot add gst appsrc into encoder pipeline.";
    gst_object_unref(impl_->appsrc_);
    impl_->appsrc_ = NULL;
    return false;
  }
  appsrc_caps = gst_caps_new_simple(
      "video/x-raw-yuv",
      "format", GST_TYPE_FOURCC, GST_MAKE_FOURCC('I', '4', '2', '0'),
      "width", G_TYPE_INT, impl_->view_size_.width(),
      "height", G_TYPE_INT, impl_->view_size_.height(),
      "framerate", GST_TYPE_FRACTION, 30, 1,
      NULL);
  if (!appsrc_caps) {
    LOG(ERROR) << __FUNCTION__ << " cannot create appsrc caps";
    return false;
  }
  gst_app_src_set_callbacks(GST_APP_SRC(impl_->appsrc_), &appsrc_callbacks,
                            static_cast<gpointer>(impl_), NULL);
  gst_app_src_set_max_bytes(GST_APP_SRC(impl_->appsrc_), max_input_buffer);
  gst_app_src_set_caps(GST_APP_SRC(impl_->appsrc_), appsrc_caps);

  // appsink initialization
  if (!(gst_appsink = gst_element_factory_make("appsink", "sink"))) {
    LOG(ERROR) << __FUNCTION__ << " cannot create appsink";
    return false;
  }
  if (!gst_bin_add(GST_BIN(gst_pipeline.get()), gst_appsink)) {
    LOG(ERROR) << __FUNCTION__ << "cannot add gst appsink into encoder pipeline.";
    gst_object_unref(gst_appsink);
    return false;
  }
  gst_app_sink_set_callbacks(GST_APP_SINK(gst_appsink), &appsink_callbacks,
                             static_cast<gpointer>(impl_), NULL);
  gst_app_sink_set_max_buffers(GST_APP_SINK(gst_appsink), 1);

#ifdef OS_TIZEN
  DVLOG(1) << "######################################";
  DVLOG(1) << "      USING omx_h264enc ENCODER";
  DVLOG(1) << "######################################";

  // encoder initialization
  if (!(impl_->encoder_ = gst_element_factory_make("omx_h264enc", "encoder"))) {
    LOG(ERROR) << __FUNCTION__ << " cannot create omx_h264enc encoder.";
    return false;
  }
  if(!gst_bin_add(GST_BIN(gst_pipeline.get()), impl_->encoder_)) {
    LOG(ERROR) << __FUNCTION__ << " cannot add omx_h264enc into encoder pipeline.";
    gst_object_unref(impl_->encoder_);
    impl_->encoder_ = NULL;
    return false;
  }
  g_object_set(impl_->encoder_,
               "byte-stream", TRUE,
               "control-rate", 1, // 1:VBR_CFR  2:CBR_CFR  3:VBR_VFR  4:CBR_VFR
               "encoder-profile", 1, // BASELINE_PROFILE
               "bitrate", impl_->gst_bitrate_,
               NULL);

  // converter initialization
  if (!(gst_converter = gst_element_factory_make("c2dconvert", "cvt"))) {
    LOG(ERROR) << __FUNCTION__ << " cannot create c2dconvert.";
    return false;
  }
  if(!gst_bin_add(GST_BIN(gst_pipeline.get()), gst_converter)) {
    LOG(ERROR) << __FUNCTION__ << " cannot add c2dconvert into encoder pipeline.";
    gst_object_unref(gst_converter);
    return false;
  }

  g_object_set(G_OBJECT(impl_->appsrc_),
               "is-live", TRUE,
               "block", FALSE,
               "do-timestamp", TRUE,
               "min-latency", (gint64)(0),
               "max-latency", (gint64)(0),
               "min-percent", 80, // if buffer below 80%, need-data emits.
               "stream-type", GST_APP_STREAM_TYPE_STREAM,
               "format", GST_FORMAT_DEFAULT,
               NULL);

  g_object_set(gst_appsink, "sync", FALSE, NULL);

  converter_caps = gst_caps_new_simple(
      "video/x-raw-yuv",
      "format", GST_TYPE_FOURCC, GST_MAKE_FOURCC ('I', '4', '2', '0'),
      "width", G_TYPE_INT, impl_->view_size_.width(),
      "height", G_TYPE_INT, impl_->view_size_.height(),
      "framerate", GST_TYPE_FRACTION, 30, 1,
      NULL);
  if (!converter_caps) {
    LOG(ERROR) << __FUNCTION__ << " cannot create converter_caps.";
    return false;
  }
  retval = gst_element_link_filtered(impl_->appsrc_, gst_converter,
                                     converter_caps);
  if (retval == FALSE) {
    LOG(ERROR) << __FUNCTION__ << " cannot link appsrc_ with converter_.";
    return false;
  }

  if (!gst_element_link_many(gst_converter, impl_->encoder_,
                             gst_appsink, NULL)) {
    LOG(ERROR) << __FUNCTION__ << " cannot link converter_ with encoder_.";
    return false;
  }
#else
  DVLOG(1) << "######################################";
  DVLOG(1) << "      USING x264enc ENCODER";
  DVLOG(1) << "######################################";
  GstElement* gst_parser = NULL;

  // parser initialization
  if (!(gst_parser = gst_element_factory_make("videoparse", "parse"))) {
    LOG(ERROR) << __FUNCTION__ << " cannot create videoparse.";
    return false;
  }
  if(!gst_bin_add(GST_BIN(gst_pipeline.get()), gst_parser)) {
    LOG(ERROR) << __FUNCTION__ << " cannot add videoparse into encoder pipeline.";
    gst_object_unref(gst_parser);
    return false;
  }
  g_object_set(gst_parser,
               "format", GST_VIDEO_FORMAT_I420,
               "width", impl_->view_size_.width(),
               "height", impl_->view_size_.height(),
               "framerate", INITIAL_FRAMERATE, 1,
               NULL);

  if (!(impl_->encoder_ = gst_element_factory_make ("x264enc","encoder"))) {
    LOG(ERROR) << __FUNCTION__ << " cannot create x264enc encoder.";
    return false;
  }
  if(!gst_bin_add(GST_BIN(gst_pipeline.get()), impl_->encoder_)) {
    LOG(ERROR) << __FUNCTION__ << " cannot add x264enc into encoder pipeline.";
    gst_object_unref(impl_->encoder_);
    return false;
  }
  g_object_set(impl_->encoder_,
               "byte-stream", TRUE,
               "bitrate", impl_->gst_bitrate_,
               "tune",0x00000004,"profile", 1, NULL);

  g_object_set(G_OBJECT(impl_->appsrc_),
               "is-live", TRUE,
               "block", FALSE,
               "do-timestamp", TRUE,
               "stream-type", 0,
               "min-latency", (gint64)(0),
               "max-latency", (gint64)(0),
               "format", GST_FORMAT_TIME,
               NULL);

  g_object_set(gst_appsink, "sync", FALSE, NULL);

  if (!gst_element_link_many(impl_->appsrc_, gst_parser,
                             impl_->encoder_, gst_appsink, NULL)) {
    LOG(ERROR) << __FUNCTION__ << " cannot link for encoder pipeline.";
    return false;
  }
#endif

  if (GST_STATE_CHANGE_FAILURE ==
         gst_element_set_state(gst_pipeline.get(), GST_STATE_PLAYING)) {
    LOG(ERROR) << __FUNCTION__ << " cannot start encoder pipeline.";
    return false;
  }

  GST_DEBUG_BIN_TO_DOT_FILE(
      GST_BIN(gst_pipeline.get()), GST_DEBUG_GRAPH_SHOW_ALL, "encoder_graph.dot");

  impl_->pipeline_ = gst_pipeline.release();
  return true;
}

}  // namespace content
