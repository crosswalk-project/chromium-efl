// Copyright 2014 Samsung Electronics Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/common/gpu/media/tizen/tizen_video_decode_accelerator.h"

#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <gst/app/gstappsrc.h>
#include <gst/interfaces/xoverlay.h>
#include <gst/video/gstvideosink.h>
#include <gst/video/video.h>

#include "base/bind.h"
#include "base/memory/shared_memory.h"
#include "base/message_loop/message_loop_proxy.h"
#include "base/synchronization/waitable_event.h"
#include "base/time/time.h"

using media::VideoFrame;

namespace {

struct GstElementDeleter {
  void operator()(GstElement* ptr) const {
    DCHECK(ptr != NULL);
    gst_object_unref(ptr);
  }
};

} // namespace

namespace content {

enum {
  MAX_BITRATE = 2000000,                 // bps.
  INPUT_BUFFER_SIZE = MAX_BITRATE / 8,   // bytes. 1 sec for H.264 HD video.
};

media::VideoDecodeAccelerator* CreateTizenVideoDecodeAccelerator() {
  return new TizenVideoDecodeAccelerator();
}

struct TizenVideoDecodeAccelerator::BitstreamBufferRef {
  BitstreamBufferRef(
      base::WeakPtr<media::VideoDecodeAccelerator::Client> client,
      const scoped_refptr<base::MessageLoopProxy>& client_message_loop_proxy,
      base::SharedMemory* shm,
      size_t size,
      int32 input_id)
      : client_(client),
        client_message_loop_proxy_(client_message_loop_proxy),
        shm_(shm),
        size_(size),
        bytes_used_(0),
        input_id_(input_id),
        gst_buffer_(NULL) {}

  ~BitstreamBufferRef() {
    if (input_id_ >= 0) {
      client_message_loop_proxy_->PostTask(
          FROM_HERE,
          base::Bind(
              &media::VideoDecodeAccelerator::Client::NotifyEndOfBitstreamBuffer,
              client_,
              input_id_));
    }
  }

  static void Destruct(gpointer data) {
    DCHECK(data != NULL);
    BitstreamBufferRef* pRef = static_cast<BitstreamBufferRef*>(data);
    delete pRef;
  }

  base::WeakPtr<media::VideoDecodeAccelerator::Client> client_;
  scoped_refptr<base::MessageLoopProxy> client_message_loop_proxy_;
  scoped_ptr<base::SharedMemory> shm_;
  size_t size_;
  off_t bytes_used_;
  int32 input_id_;
  GstBuffer* gst_buffer_;
};

struct TizenVideoDecodeAccelerator::Impl {
  Impl()
      : can_feed_(true),
        is_destroying_(false),
        pipeline_(NULL),
        sink_(NULL),
        appsrc_(NULL),
        io_message_loop_proxy_(base::MessageLoopProxy::current()),
        gst_thread_("TizenDecoderThreadGst") {}

  static GstBusSyncReply OnBusMessage(
      GstBus* bus, GstMessage* msg, gpointer data) {
    switch (GST_MESSAGE_TYPE(msg)) {
      case GST_MESSAGE_ERROR: {
        gchar* debug = NULL;
        GError* error = NULL;
        gst_message_parse_error(msg, &error, &debug);
        LOG(ERROR) << __FUNCTION__
                   << " GSTError happens from bus at "
                   << GST_OBJECT_NAME(msg->src)
                   << ":" << error->message;
        LOG(ERROR) << __FUNCTION__
                   << " Debugging Info: "
                   << (debug != NULL ? debug : "none");
        g_error_free(error);
        g_free(debug);
        break;
      }
    }
    return GST_BUS_PASS;
  }

  static void StartFeed(GstAppSrc *source, guint size, gpointer app) {
    DCHECK(source);
    content::TizenVideoDecodeAccelerator::Impl* impl =
        static_cast<content::TizenVideoDecodeAccelerator::Impl*>(app);
    impl->can_feed_ = true;
  }

  static void StopFeed(GstAppSrc *source, gpointer app) {
    DCHECK(source);
    content::TizenVideoDecodeAccelerator::Impl* impl =
        static_cast<content::TizenVideoDecodeAccelerator::Impl*>(app);
    impl->can_feed_ = false;
  }

  volatile bool can_feed_;
  volatile bool is_destroying_;
  GstElement* pipeline_;
  GstElement* sink_;
  GstElement* appsrc_;
  scoped_refptr<base::MessageLoopProxy> io_message_loop_proxy_;
  scoped_ptr<base::WeakPtrFactory<Client> > io_client_weak_factory_;
  base::Thread gst_thread_;
};

TizenVideoDecodeAccelerator::TizenVideoDecodeAccelerator()
    : impl_(NULL) {
}

TizenVideoDecodeAccelerator::~TizenVideoDecodeAccelerator() {
}

bool TizenVideoDecodeAccelerator::Initialize(
    media::VideoCodecProfile profile,
    Client* client) {
  GError* error = NULL;
  GstCaps* video_caps = NULL;
  GstElement* gst_decoder = NULL;
  GstBus* gst_bus = NULL;
  scoped_ptr<GstElement, GstElementDeleter> gst_pipeline;
  static GstAppSrcCallbacks appsrc_callbacks =
      {&Impl::StartFeed, &Impl::StopFeed, NULL};
  CHECK(impl_ == NULL);
  impl_ = new Impl();
  impl_->io_client_weak_factory_.reset(
      new base::WeakPtrFactory<Client>(client));

  switch (profile) {
    case media::H264PROFILE_BASELINE:
      DVLOG(1) << "Initialize(): profile -> H264PROFILE_BASELINE";
      break;
    case media::H264PROFILE_MAIN:
      DVLOG(1) << "Initialize(): profile -> H264PROFILE_MAIN";
      break;
    default:
      LOG(ERROR) << "Initialize(): unsupported profile=" << profile;
      return false;
  };

  if (!gst_is_initialized() && !gst_init_check(NULL, NULL, &error)) {
    LOG(ERROR) << __FUNCTION__ << "cannot initialize gstreamer.";
    g_error_free(error);
    return false;
  }

  // pipeline initialization.
  gst_pipeline.reset(gst_pipeline_new("h264_decode"));
  if (!gst_pipeline) {
    LOG(ERROR) << __FUNCTION__ << "cannot initialize gst pipeline.";
    return false;
  }
  if (!(gst_bus = gst_pipeline_get_bus(GST_PIPELINE(gst_pipeline.get())))) {
    return false;
  }
  gst_bus_set_sync_handler(gst_bus, Impl::OnBusMessage, impl_);
  gst_object_unref(gst_bus);

  // appsrc initialization.
  if (!(impl_->appsrc_ = gst_element_factory_make("appsrc", "src"))) {
    LOG(ERROR) << __FUNCTION__ << "cannot initialize gst appsrc.";
    return false;
  }
  if (!gst_bin_add(GST_BIN(gst_pipeline.get()), impl_->appsrc_)) {
    gst_object_unref(impl_->appsrc_);
    impl_->appsrc_ = NULL;
    return false;
  }
  gst_app_src_set_max_bytes(GST_APP_SRC(impl_->appsrc_), INPUT_BUFFER_SIZE);
  gst_app_src_set_callbacks(GST_APP_SRC(impl_->appsrc_), &appsrc_callbacks,
                            static_cast<gpointer>(impl_), NULL);
  g_object_set(G_OBJECT(impl_->appsrc_),
               "is-live", TRUE,
               "block", FALSE,
               "min-percent", 80, // if buffer below 80%, need-data emits.
               "stream-type", GST_APP_STREAM_TYPE_STREAM,
               NULL);
  if (!(video_caps = gst_caps_from_string("video/x-h264,framerate=30/1"))) {
    return false;
  }
  g_object_set(G_OBJECT(impl_->appsrc_), "caps", video_caps, NULL);
  gst_caps_unref(video_caps);

#if defined(OS_TIZEN)
  DVLOG(1) << "######################################";
  DVLOG(1) << "      USING omx_h264dec DECODER " << (unsigned int)this;
  DVLOG(1) << "######################################";
  // decoder initialization.
  if (!(gst_decoder = gst_element_factory_make("omx_h264dec", "decoder"))) {
    LOG(ERROR) << __FUNCTION__ << " cannot create omx_h264dec.";
    return false;
  }
  if (!gst_bin_add(GST_BIN(gst_pipeline.get()), gst_decoder)) {
    gst_object_unref(gst_decoder);
    return false;
  }

  // sink initialization.
  if (!(impl_->sink_ = gst_element_factory_make("xvimagesink", "xvimagesink"))) {
    LOG(ERROR) << __FUNCTION__ << " cannot create xvimagesink.";
    return false;
  }
  if (!gst_bin_add(GST_BIN(gst_pipeline.get()), impl_->sink_)) {
    gst_object_unref(impl_->sink_);
    impl_->sink_ = NULL;
    return false;
  }
  g_object_set(impl_->sink_, "rotate", 0, NULL);

  // linking the elements.
  if (!gst_element_link(impl_->appsrc_, gst_decoder)) {
    LOG(ERROR) << __FUNCTION__ << " Source and Decoder could not be linked";
    return false;
  }
  if (!gst_element_link(gst_decoder, impl_->sink_)) {
    LOG(ERROR) << __FUNCTION__ << " Decoder and Sink could not be linked";
    return false;
  }

#else
  DVLOG(1) << "######################################";
  DVLOG(1) << "      USING ffdec_h264 DECODER";
  DVLOG(1) << "######################################";
  GstElement* gst_colorspace = NULL;

  // decoder initialization
  if (!(gst_decoder = gst_element_factory_make("ffdec_h264", "H264-decoder"))) {
    LOG(ERROR) << __FUNCTION__ << " cannot create ffdec_h264.";
    return false;
  }
  if (!gst_bin_add(GST_BIN(gst_pipeline.get()), gst_decoder)) {
    gst_object_unref(gst_decoder);
    return false;
  }

  // colorspace initialization
  if (!(gst_colorspace = gst_element_factory_make("ffmpegcolorspace", "cs"))) {
    LOG(ERROR) << __FUNCTION__ << " cannot create ffmpegcolorspace.";
    return false;
  }
  if (!gst_bin_add(GST_BIN(gst_pipeline.get()), gst_colorspace)) {
    gst_object_unref(gst_colorspace);
    return false;
  }

  if (!(impl_->sink_ = gst_element_factory_make("autovideosink", "sink"))) {
    LOG(ERROR) << __FUNCTION__ << " cannot create autovideosink.";
    return false;
  }
  if (!gst_bin_add(GST_BIN(gst_pipeline.get()), impl_->sink_)) {
    gst_object_unref(impl_->sink_);
    impl_->sink_ = NULL;
    return false;
  }

  if(!gst_element_link_many(impl_->appsrc_, gst_decoder, gst_colorspace,
                            impl_->sink_, NULL)) {
    LOG(ERROR) << __FUNCTION__ << " Some element could not be linked";
    return false;
  }
#endif
  if (!impl_->gst_thread_.Start()) {
    LOG(ERROR) << __FUNCTION__ << " gst_thread_ failed to start";
    return false;
  }

  impl_->gst_thread_.message_loop()->PostTask(
      FROM_HERE,
      base::Bind(&TizenVideoDecodeAccelerator::StartDecoder,
      base::Unretained(this)));

  GST_DEBUG_BIN_TO_DOT_FILE(
      GST_BIN(gst_pipeline.get()), GST_DEBUG_GRAPH_SHOW_ALL, "decoder_graph.dot");

  impl_->pipeline_ = gst_pipeline.release();
  return true;
}

void TizenVideoDecodeAccelerator::Decode(
    const media::BitstreamBuffer& bitstream_buffer) {
  scoped_ptr<BitstreamBufferRef> buffer_ref;
  scoped_ptr<base::SharedMemory> shm(
      new base::SharedMemory(bitstream_buffer.handle(), true));

  if (!shm->Map(bitstream_buffer.size())) {
    LOG(ERROR) << __FUNCTION__ << " could not map bitstream_buffer";
    NotifyError(media::VideoDecodeAccelerator::UNREADABLE_INPUT);
    return;
  }

  buffer_ref.reset(new BitstreamBufferRef(
      impl_->io_client_weak_factory_->GetWeakPtr(),
      base::MessageLoopProxy::current(),
      shm.release(),
      bitstream_buffer.size(),
      bitstream_buffer.id()));

  if (!buffer_ref) {
    return;
  }

  if (impl_->can_feed_ && !impl_->is_destroying_) {
    impl_->gst_thread_.message_loop()->PostTask(
        FROM_HERE,
        base::Bind(&TizenVideoDecodeAccelerator::OnDecode,
                   base::Unretained(this),
                   base::Passed(&buffer_ref)));
  } else {
    DVLOG(2) << __FUNCTION__
             << " Frame drop on decoder:"
             << " INPUT Q is FULL";
  }
}

void TizenVideoDecodeAccelerator::AssignPictureBuffers(
    const std::vector<media::PictureBuffer>& buffers) {
  NOTIMPLEMENTED();
}

void TizenVideoDecodeAccelerator::ReusePictureBuffer(
    int32 picture_buffer_id) {
  NOTIMPLEMENTED();
}

void TizenVideoDecodeAccelerator::Flush() {
  NOTIMPLEMENTED();
}

void TizenVideoDecodeAccelerator::Reset() {
  NOTIMPLEMENTED();
}

void TizenVideoDecodeAccelerator::Destroy() {
  if (impl_ != NULL) {
    if (impl_->gst_thread_.IsRunning()) {
      impl_->gst_thread_.Stop();
    }
    gst_app_src_end_of_stream(GST_APP_SRC(impl_->appsrc_));
    impl_->is_destroying_ = true;
    if (impl_->pipeline_) {
      gst_element_set_state(impl_->pipeline_, GST_STATE_NULL);
      gst_object_unref(GST_OBJECT(impl_->pipeline_));
    }
    delete impl_;
    impl_ = NULL;
  }
  delete this;
}

bool TizenVideoDecodeAccelerator::CanDecodeOnIOThread(){
  return false;
}

void TizenVideoDecodeAccelerator::StartDecoder() {
  gst_element_set_state(impl_->pipeline_, GST_STATE_PLAYING);
};

void TizenVideoDecodeAccelerator::OnDecode(
    scoped_ptr<BitstreamBufferRef> buffer_ref) {
  if (!buffer_ref) {
    return;
  }

  if (!(buffer_ref->gst_buffer_ = gst_buffer_new())) {
    return;
  }

  GST_BUFFER_MALLOCDATA(buffer_ref->gst_buffer_) =
      reinterpret_cast<guint8*>(buffer_ref.get());
  GST_BUFFER_FREE_FUNC(buffer_ref->gst_buffer_) = BitstreamBufferRef::Destruct;
  GST_BUFFER_SIZE(buffer_ref->gst_buffer_) = buffer_ref->size_;
  GST_BUFFER_DATA(buffer_ref->gst_buffer_) =
      static_cast<guint8*>(buffer_ref->shm_->memory());

  if (GST_FLOW_OK !=
          gst_app_src_push_buffer(GST_APP_SRC(impl_->appsrc_),
                                  buffer_ref->gst_buffer_)) {
    LOG(ERROR) << __FUNCTION__ << " fail to push buffer into decoder pipeline";
    return;
  }

  // lifecycle of buffer_ref will be handled by gstreamer.
  buffer_ref.release();
}

void TizenVideoDecodeAccelerator::NotifyError(
    media::VideoDecodeAccelerator::Error error) {
  if (impl_->io_client_weak_factory_->GetWeakPtr()) {
    impl_->io_client_weak_factory_->GetWeakPtr()->NotifyError(error);
  }
}

}  // namespace content
