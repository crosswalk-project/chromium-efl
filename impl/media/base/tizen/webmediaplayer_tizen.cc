// Copyright 2014 Samsung Electronics Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/tizen/webmediaplayer_tizen.h"

#include <gst/gst.h>

#include "cc/blink/web_layer_impl.h"
#include "cc/layers/video_layer.h"
#include "content/renderer/media/render_media_log.h"
#include "content/renderer/media/renderer_gpu_video_accelerator_factories.h"
#include "content/renderer/render_frame_impl.h"
#include "content/renderer/render_thread_impl.h"
#include "media/base/tizen/media_player_tizen.h"
#include "media/base/bind_to_current_loop.h"
#include "media/base/video_frame.h"
#include "media/blink/webmediaplayer_util.h"
#include "third_party/WebKit/public/platform/WebMediaPlayerClient.h"

#define BIND_TO_RENDER_LOOP(function) \
  (DCHECK(main_loop_->BelongsToCurrentThread()), \
  media::BindToCurrentLoop(base::Bind(function, AsWeakPtr())))

namespace media {

// fourcc for gst-video-format
const uint32 GST_VIDEO_SN12 = GST_MAKE_FOURCC('S','N','1','2');
const uint32 GST_VIDEO_I420 = GST_MAKE_FOURCC('I','4','2','0');
const uint32 GST_VIDEO_NV12 = GST_MAKE_FOURCC('N','V','1','2');

// tile size for SN12
const uint SN12_TILE_WIDTH = 64;
const uint SN12_TILE_HEIGHT = 32;
const uint SN12_TILE_SIZE = SN12_TILE_WIDTH * SN12_TILE_HEIGHT;

// Removes query string from URI
GURL GetCleanURL(std::string url) {
  // FIXME: Need to consider "app://" scheme.
  if (!url.compare(0, 7, "file://")) {
    int position = url.find("?");
    if(position != -1)
      url.erase(url.begin() + position, url.end());
  }
  GURL url_(url);
  return url_;
}

WebMediaPlayerTizen::WebMediaPlayerTizen(
    content::RendererMediaPlayerManagerTizen* manager,
    blink::WebFrame* frame,
    blink::WebMediaPlayerClient* client,
    base::WeakPtr<media::WebMediaPlayerDelegate> delegate)
    : frame_(frame),
      network_state_(blink::WebMediaPlayer::NetworkStateEmpty),
      ready_state_(blink::WebMediaPlayer::ReadyStateHaveNothing),
      main_loop_(base::MessageLoopProxy::current()),
      media_task_runner_(
          content::RenderThreadImpl::current()->GetMediaThreadTaskRunner()),
      manager_(manager),
      client_(client),
      media_log_(new content::RenderMediaLog()),
      delegate_(delegate),
      compositor_(new media::VideoFrameCompositor(
          BIND_TO_RENDER_LOOP(&WebMediaPlayerTizen::OnNaturalSizeChanged),
          BIND_TO_RENDER_LOOP(&WebMediaPlayerTizen::OnOpacityChanged))),
      weak_factory_(this),
      gst_video_format_(0),
      audio_(false),
      video_(false),
      current_time_(0),
      duration_(0),
      is_paused_(true),
      is_seeking_(false),
      seek_time_(0),
      pending_seek_(0),
      pending_seek_time_(0),
      opaque_(false),
      natural_size_(0, 0),
      did_loading_progress_(false),
      volume_(0),
      gpu_factories_(content::RenderThreadImpl::current()->GetGpuFactories()) {
  DCHECK(manager_);
  VLOG(1) << "WebMediaPlayerTizen::" << __FUNCTION__;
  FrameAvailable_ = false;
  // We want to be notified of |main_loop_| destruction.
  base::MessageLoop::current()->AddDestructionObserver(this);

  player_id_ = manager_->RegisterMediaPlayer(this);

  // Threaded compositing isn't enabled universally yet.
  if (!compositor_task_runner_.get())
    compositor_task_runner_ = base::MessageLoopProxy::current();

  media_log_->AddEvent(
      media_log_->CreateEvent(media::MediaLogEvent::WEBMEDIAPLAYER_CREATED));
}

WebMediaPlayerTizen::~WebMediaPlayerTizen() {
  VLOG(1) << "WebMediaPlayerTizen::" << __FUNCTION__;
  if (manager_) {
    manager_->DestroyPlayer(player_id_);
    manager_->UnregisterMediaPlayer(player_id_);
  }

  SetVideoFrameProviderClient(NULL);
  client_->setWebLayer(NULL);
  if (delegate_.get())
    delegate_->PlayerGone(this);
  if (base::MessageLoop::current())
    base::MessageLoop::current()->RemoveDestructionObserver(this);
    compositor_task_runner_->DeleteSoon(FROM_HERE, compositor_);
}

void WebMediaPlayerTizen::load(LoadType load_type,
    const blink::WebURL& url,
    CORSMode cors_mode) {
  VLOG(1) << "WebMediaPlayerTizen::" << __FUNCTION__ << " load type - "
          << load_type;
  int demuxer_client_id = 0;
  if (load_type == LoadTypeMediaSource) {
    // FIXME: EFL GST-package on desktop cannot handle AAC decoding.
    // Disabling MSE for desktop.
#ifdef OS_TIZEN
    player_type_ = MEDIA_PLAYER_TYPE_MEDIA_SOURCE;
    content::RendererDemuxerTizen* demuxer =
        content::RenderThreadImpl::current()->renderer_demuxer();
    demuxer_client_id = demuxer->GetNextDemuxerClientID();
    media_source_delegate_.reset(new content::MediaSourceDelegateTizen(
        demuxer, demuxer_client_id, media_task_runner_, media_log_.get()));
    media_source_delegate_->InitializeMediaSource(
        base::Bind(&WebMediaPlayerTizen::OnMediaSourceOpened,
                   weak_factory_.GetWeakPtr()),
        base::Bind(&WebMediaPlayerTizen::SetNetworkState,
                   weak_factory_.GetWeakPtr()),
        base::Bind(&WebMediaPlayerTizen::OnDurationChange,
                   weak_factory_.GetWeakPtr()));
#else
    // Posting Error Message to HTMLMediaElement.
    SetNetworkState(WebMediaPlayer::NetworkStateDecodeError);
#endif
  } else if (load_type == LoadTypeURL) {
    player_type_ = MEDIA_PLAYER_TYPE_URL;
  } else {
    LOG(ERROR) << "Unsupported load type " << load_type;
    return;
  }

  // FIXME: Check URL, Volume for MS.
  manager_->Initialize(
      player_id_, player_type_, GetCleanURL(url.string().utf8()),
      volume_, demuxer_client_id);
}

void WebMediaPlayerTizen::OnMediaSourceOpened(
    blink::WebMediaSource* web_media_source) {
  client_->mediaSourceOpened(web_media_source);
}

void WebMediaPlayerTizen::play() {
  manager_->Play(player_id_);
  // Has to be updated from |MediaPlayerBridgeGstreamer| but IPC causes delay.
  // There are cases were play - pause are fired successively and would fail.
  is_paused_ = false;
}

void WebMediaPlayerTizen::pause() {
  manager_->Pause(player_id_, true);
  // Has to be updated from |MediaPlayerBridgeGstreamer| but IPC causes delay.
  // There are cases were play - pause are fired successively and would fail.
  is_paused_ = true;
}

void WebMediaPlayerTizen::RequestPause() {
  pause();
  client_->playbackStateChanged();
}

bool WebMediaPlayerTizen::supportsSave() const {
  return false;
}

void WebMediaPlayerTizen::seek(double seconds) {
  VLOG(1) << "WebMediaPlayerTizen::" << __FUNCTION__ <<  " : " << seconds
          << " ID " << player_id_;
  DCHECK(main_loop_->BelongsToCurrentThread());

  if (is_seeking_) {
    if (seconds == seek_time_) {
      if (media_source_delegate_) {
        if (!pending_seek_) {
          // If using media source demuxer, only suppress redundant seeks if
          // there is no pending seek. This enforces that any pending seek that
          // results in a demuxer seek is preceded by matching
          // CancelPendingSeek() and StartSeek() calls.
          return;
        }
      } else {
        // Suppress all redundant seeks if unrestricted by media source
        // demuxer API.
        pending_seek_ = false;
        return;
      }
    }

    pending_seek_ = true;
    pending_seek_time_ = seconds;
    if (media_source_delegate_)
      media_source_delegate_->CancelPendingSeek(
          media::ConvertSecondsToTimestamp(pending_seek_time_));
    // Later, OnSeekComplete will trigger the pending seek.
    return;
  }

  is_seeking_ = true;
  seek_time_ = seconds;

  // Once Chunk demuxer seeks GST seek will be intiated.
  if (media_source_delegate_)
    media_source_delegate_->StartWaitingForSeek(
        media::ConvertSecondsToTimestamp(seek_time_));
  manager_->Seek(player_id_, seek_time_);

  // Draw empty frame during seek.
  if (video_) {
    gfx::Size size(gst_width_, gst_height_);
    scoped_refptr<VideoFrame> video_frame = VideoFrame::CreateBlackFrame(size);
    FrameReady(video_frame);
  }
}

void WebMediaPlayerTizen::setRate(double rate) {
  manager_->SetRate(player_id_, rate);
}

void WebMediaPlayerTizen::setVolume(double volume) {
  volume_ = volume;
  manager_->SetVolume(player_id_, volume);
}

blink::WebTimeRanges WebMediaPlayerTizen::buffered() const{
  return buffered_;
}

blink::WebTimeRanges WebMediaPlayerTizen::seekable() const {
  const double seekable_end = duration();
  if (std::isinf(seekable_end))
    return blink::WebTimeRanges();

  blink::WebTimeRange seekable_range(0.0, seekable_end);
  return blink::WebTimeRanges(&seekable_range, 1);
}

void WebMediaPlayerTizen::paint(blink::WebCanvas* canvas,
    const blink::WebRect& rect,
    unsigned char alpha,
    SkXfermode::Mode mode) {
  scoped_refptr<media::VideoFrame> video_frame =
      GetCurrentFrameFromCompositor();

  gfx::Rect gfx_rect(rect);
  skcanvas_video_renderer_.Paint(
      video_frame.get(), canvas, gfx_rect, alpha,
      SkXfermode::kSrcOver_Mode, media::VIDEO_ROTATION_0);
}

bool WebMediaPlayerTizen::hasVideo() const {
  return video_;
}

bool WebMediaPlayerTizen::hasAudio() const {
  return audio_;
}

blink::WebSize WebMediaPlayerTizen::naturalSize() const {
  return blink::WebSize(natural_size_);
}

bool WebMediaPlayerTizen::paused() const {
  return is_paused_;
}

bool WebMediaPlayerTizen::seeking() const {
  return is_seeking_;
}

double WebMediaPlayerTizen::duration() const {
  return duration_;
}

double WebMediaPlayerTizen::currentTime() const {
  if (seeking())
    return pending_seek_ ? pending_seek_time_ : seek_time_;
  return current_time_;
}

blink::WebMediaPlayer::NetworkState WebMediaPlayerTizen::networkState() const {
  return network_state_;
}

blink::WebMediaPlayer::ReadyState WebMediaPlayerTizen::readyState() const {
  return ready_state_;
}

bool WebMediaPlayerTizen::didLoadingProgress() {
  if (did_loading_progress_) {
    did_loading_progress_ = false;
    return true;
  }
  return false;
}

bool WebMediaPlayerTizen::hasSingleSecurityOrigin() const {
  return true;
}

bool WebMediaPlayerTizen::didPassCORSAccessCheck() const {
  return false;
}

double WebMediaPlayerTizen::mediaTimeForTimeValue(double timeValue) const {
  return media::ConvertSecondsToTimestamp(timeValue).InSecondsF();
}

void WebMediaPlayerTizen::SetVideoFrameProviderClient(
    cc::VideoFrameProvider::Client* client) {
  // This is called from both the main renderer thread and the compositor
  // thread (when the main thread is blocked).
  compositor_->SetVideoFrameProviderClient(client);
}

scoped_refptr<media::VideoFrame>WebMediaPlayerTizen::GetCurrentFrame() {
  scoped_refptr<media::VideoFrame> current_frame =
      GetCurrentFrameFromCompositor();
  return current_frame;
}

void WebMediaPlayerTizen::SetReadyState(WebMediaPlayer::ReadyState state) {
  ready_state_ = state;
  client_->readyStateChanged();
}

void WebMediaPlayerTizen::SetNetworkState(WebMediaPlayer::NetworkState state) {
  network_state_ = state;
  client_->networkStateChanged();
}

void WebMediaPlayerTizen::OnNewFrameAvailable(base::SharedMemoryHandle Handle,
    uint32 yuv_size,
    base::TimeDelta timestamp) {
  base::SharedMemory shared_memory(Handle, false);
  shared_memory.Map(yuv_size);
  uint8* const yuv_buffer = static_cast<uint8*>(shared_memory.memory());

  gfx::Size size(gst_width_, gst_height_);
  scoped_refptr<VideoFrame> video_frame =
      VideoFrame::CreateFrame(
          VideoFrame::YV12, size, gfx::Rect(size), size, timestamp);

  // decoded format is SN12 on Tizen device
  // video format converted from SN12 to YV12
  uint8* gst_buf = yuv_buffer;
  switch(gst_video_format_) {
    case GST_VIDEO_I420: {
      const uint c_frm_size = yuv_size / 6;
      const uint y_frm_size = c_frm_size << 2;  // * 4;

      // kYPlane
      memcpy(video_frame.get()->data(VideoFrame::kYPlane),
             gst_buf, y_frm_size);

      gst_buf += y_frm_size;
      uint8* gst_buf2 = gst_buf + c_frm_size;
      uint8* u_plane = video_frame.get()->data(VideoFrame::kUPlane);
      uint8* v_plane = video_frame.get()->data(VideoFrame::kVPlane);
      const uint uv_stride = video_frame.get()->stride(VideoFrame::kUPlane);
      const uint uv_rows = video_frame.get()->rows(VideoFrame::kUPlane);
      const uint gst_stride = c_frm_size / uv_rows;

      for(uint row=0; row<uv_rows; ++row) {
        // kUPlane
        memcpy(u_plane, gst_buf, gst_stride);
        u_plane += uv_stride;
        gst_buf += gst_stride;

        // kVPlane
        memcpy(v_plane, gst_buf2, gst_stride);
        v_plane += uv_stride;
        gst_buf2 += gst_stride;
      }

      break;
    }
    case GST_VIDEO_SN12: {
      //const uint tile_w = (gst_width_ - 1) / SN12_TILE_WIDTH + 1;
      //const uint tile_w_align = (tile_w + 1) & ~1;
      //const uint tile_h_luma = (gst_height_ - 1) / SN12_TILE_HEIGHT + 1;
      const uint tile_w_align = ((gst_width_ - 1) / SN12_TILE_WIDTH + 2) & ~1;
      const uint tile_luma_stride = tile_w_align * SN12_TILE_WIDTH;
      uint luma_size = SN12_TILE_SIZE * tile_w_align
                       * ((gst_height_-1)/SN12_TILE_HEIGHT + 1);

      uint8* y_frm = video_frame.get()->data(VideoFrame::kYPlane);
      const uint y_stride = video_frame.get()->stride(VideoFrame::kYPlane);

      // actually copy and convert luma buffer
      for(int i=0; i<gst_height_; i++) {
        memcpy(y_frm, gst_buf, gst_width_);
        y_frm += y_stride;
        gst_buf += tile_luma_stride;
      }

      gst_buf = yuv_buffer + luma_size;
      uint8* gst_buf2 = gst_buf + 1;
      uint8* u_frm = video_frame.get()->data(VideoFrame::kUPlane);
      uint8* v_frm = video_frame.get()->data(VideoFrame::kVPlane);
      const uint uv_stride = video_frame.get()->stride(VideoFrame::kUPlane);
      const uint uv_rows = video_frame.get()->rows(VideoFrame::kUPlane);

      // actually copy and convert chroma buffer
      for(uint row=0; row<uv_rows; ++row) {
        for(uint i=0; i<uv_stride; i++) {
          u_frm[i] = gst_buf[i*2];
          v_frm[i] = gst_buf2[i*2];
        }

        gst_buf += tile_luma_stride;
        gst_buf2 += tile_luma_stride;
        u_frm += uv_stride;
        v_frm += uv_stride;
      }

      break;
    }
    case GST_VIDEO_NV12: {
      const uint c_frm_size = yuv_size / 6;
      const uint y_frm_size = c_frm_size << 2;  // * 4;

      // kYPlane
      memcpy(video_frame.get()->data(VideoFrame::kYPlane),
             gst_buf, y_frm_size);

      gst_buf += y_frm_size;
      uint8* gst_buf2 = gst_buf + 1;
      uint8* u_plane = video_frame.get()->data(VideoFrame::kUPlane);
      uint8* v_plane = video_frame.get()->data(VideoFrame::kVPlane);

      for(uint i = 0; i < c_frm_size; i++){
        // kUPlane
        u_plane[i] = gst_buf[i * 2];
        // kVPlane
        v_plane[i] = gst_buf2[i * 2];
      }
      break;
    }
    default: {
      LOG(ERROR) << "WebMediaPlayerTizen::" << __FUNCTION__
          << " not supported video format";
      break;
    }
  }

  shared_memory.Close();
  FrameReady(video_frame);
}

#if defined(TIZEN_MULTIMEDIA_PIXMAP_SUPPORT)
// FIXME: Graphics team need to merge painting of Video-Frame on to 2d-canvas
// from m34.
void WebMediaPlayerTizen::OnPlatformSurfaceUpdated(
    int pixmap_id,
    base::TimeDelta timestamp) {
  gfx::Size size(gst_width_, gst_height_);
  scoped_refptr<VideoFrame> video_frame = VideoFrame::WrapNativePixmap(
      VideoFrame::NATIVE_PIXMAP, size, gfx::Rect(size), size,
      timestamp, pixmap_id);
  FrameReady(video_frame);
}
#endif

void WebMediaPlayerTizen::FrameReady(
    const scoped_refptr<media::VideoFrame>& frame) {
  compositor_task_runner_->PostTask(
      FROM_HERE,
      base::Bind(&media::VideoFrameCompositor::UpdateCurrentFrame,
                 base::Unretained(compositor_),
                 frame));
}
void WebMediaPlayerTizen::OnMediaDataChange(int format, int height, int width, int media) {
  gst_video_format_ = static_cast<uint32>(format);
  gst_height_ = height;
  gst_width_  = width;
  audio_ = media & media::MEDIA_AUDIO_MASK ? true : false;
  video_ = media & media::MEDIA_VIDEO_MASK ? true : false;
  natural_size_ = gfx::Size(width, height);
  SetReadyState(WebMediaPlayer::ReadyStateHaveMetadata);
  if (hasVideo() && !video_weblayer_.get()) {
    scoped_refptr<cc::VideoLayer> layer =
        cc::VideoLayer::Create(compositor_, media::VIDEO_ROTATION_0);
    video_weblayer_.reset(new cc_blink::WebLayerImpl(layer));
    video_weblayer_->setOpaque(opaque_);
    client_->setWebLayer(video_weblayer_.get());
  }
}

void WebMediaPlayerTizen::OnTimeChanged() {
  is_seeking_ = false;

  // Handling pending seek for ME. For MSE |CancelPendingSeek|
  // will handle the pending seeks.
  if (!media_source_delegate_ && pending_seek_) {
    pending_seek_ = false;
    seek(pending_seek_time_);
    return;
  }
  client_->timeChanged();
}

void WebMediaPlayerTizen::OnDurationChange(double duration) {
  duration_ = duration;
  client_->durationChanged();
}

void WebMediaPlayerTizen::OnNaturalSizeChanged(gfx::Size size) {
  DCHECK(main_loop_->BelongsToCurrentThread());
  DCHECK_NE(ready_state_, WebMediaPlayer::ReadyStateHaveNothing);
  TRACE_EVENT0("media", "WebMediaPlayerTizen::OnNaturalSizeChanged");
  media_log_->AddEvent(
      media_log_->CreateVideoSizeSetEvent(size.width(), size.height()));
  natural_size_ = size;

  client_->sizeChanged();
}

void WebMediaPlayerTizen::OnOpacityChanged(bool opaque) {
  DCHECK(main_loop_->BelongsToCurrentThread());
  DCHECK_NE(ready_state_, WebMediaPlayer::ReadyStateHaveNothing);

  opaque_ = opaque;
  if (video_weblayer_)
    video_weblayer_->setOpaque(opaque_);
}

static void GetCurrentFrameAndSignal(
    media::VideoFrameCompositor* compositor,
    scoped_refptr<media::VideoFrame>* video_frame_out,
    base::WaitableEvent* event) {
  TRACE_EVENT0("media", "GetCurrentFrameAndSignal");
  *video_frame_out = compositor->GetCurrentFrame();
  event->Signal();
}

scoped_refptr<media::VideoFrame>
    WebMediaPlayerTizen::GetCurrentFrameFromCompositor() {
  TRACE_EVENT0("media", "WebMediaPlayerImpl::GetCurrentFrameFromCompositor");

  if (compositor_task_runner_->BelongsToCurrentThread())
    return compositor_->GetCurrentFrame();

  // Use a posted task and waitable event instead of a lock otherwise
  // WebGL/Canvas can see different content than what the compositor is seeing.
  scoped_refptr<media::VideoFrame> video_frame;
  base::WaitableEvent event(false, false);
  compositor_task_runner_->PostTask(FROM_HERE,
                                    base::Bind(&GetCurrentFrameAndSignal,
                                               base::Unretained(compositor_),
                                               &video_frame,
                                               &event));
  event.Wait();
  return video_frame;
}

void WebMediaPlayerTizen::OnTimeUpdate(double current_time) {
  current_time_ = current_time;
}

void WebMediaPlayerTizen::OnBufferUpdate(
    std::vector<media::MediaPlayerTizen::TimeRanges> buffer_range) {
  media::Ranges<base::TimeDelta> time_ranges;
  std::vector<media::MediaPlayerTizen::TimeRanges>::iterator tr_it;
  for ( tr_it = buffer_range.begin(); tr_it != buffer_range.end(); ++tr_it) {
    time_ranges.Add(
        base::TimeDelta::FromMicroseconds((*tr_it).start),
        base::TimeDelta::FromMicroseconds((*tr_it).end));
  }
  blink::WebTimeRanges web_ranges(ConvertToWebTimeRanges(time_ranges));
  buffered_.swap(web_ranges);
  did_loading_progress_ = true;
}

void WebMediaPlayerTizen::OnPauseStateChange(bool state) {
  VLOG(1) << "WebMediaPlayerTizen::" << __FUNCTION__ << " state:" << state;
  is_paused_ = state;
  if (delegate_.get()) {
    if(is_paused_)
      delegate_->DidPause(this);
    else
      delegate_->DidPlay(this);
  }
}

void WebMediaPlayerTizen::OnSeekStateChange(bool state) {
  VLOG(1) << "WebMediaPlayerTizen::" << __FUNCTION__ << " state:" << state
          << " ID " << player_id_;
  is_seeking_ = state;
  // Draw empty frame during seek.
  if (video_ && is_seeking_) {
    gfx::Size size(gst_width_, gst_height_);
    scoped_refptr<VideoFrame> video_frame = VideoFrame::CreateBlackFrame(size);
    FrameReady(video_frame);
  }
}

void WebMediaPlayerTizen::OnRequestSeek(double seek_time) {
  client_->requestSeek(seek_time);
}

} // namespace media
