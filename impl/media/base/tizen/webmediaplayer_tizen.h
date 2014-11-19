// Copyright 2014 Samsung Electronics Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_RENDERER_MEDIA_TIZEN_WEBMEDIAPLAYER_TIZEN_H_
#define CONTENT_RENDERER_MEDIA_TIZEN_WEBMEDIAPLAYER_TIZEN_H_

#include "base/message_loop/message_loop.h"
#include "cc/layers/video_frame_provider_client_impl.h"
#include "content/renderer/media/tizen/media_source_delegate_tizen.h"
#include "content/renderer/media/tizen/renderer_media_player_manager_tizen.h"
#include "media/blink/video_frame_compositor.h"
#include "media/filters/skcanvas_video_renderer.h"

namespace blink {
class WebFrame;
}

namespace cc_blink {
class WebLayerImpl;
}

namespace content {
class RendererMediaPlayerManagerTizen;
}

namespace media {

class GpuVideoAcceleratorFactories;
class WebMediaPlayerDelegate;

// This class implements blink::WebMediaPlayer by keeping the tizen
// media player in the browser process. It listens to all the status changes
// sent from the browser process and sends playback controls to the media
// player.
class WebMediaPlayerTizen
    : public blink::WebMediaPlayer,
      public cc::VideoFrameProvider,
      public base::MessageLoop::DestructionObserver,
      public base::SupportsWeakPtr<WebMediaPlayerTizen> {
 public:
  // Construct a WebMediaPlayerTizen object. This class communicates
  // with the WebMediaPlayerTizen object in the browser process through
  // |proxy|.
  WebMediaPlayerTizen(
      content::RendererMediaPlayerManagerTizen* manager,
      blink::WebFrame* frame,
      blink::WebMediaPlayerClient* client,
      base::WeakPtr<media::WebMediaPlayerDelegate> delegate);
  virtual ~WebMediaPlayerTizen();

  // blink::WebMediaPlayer implementation.
  virtual void load(LoadType load_type,
                    const blink::WebURL& url,
                    CORSMode cors_mode) override;

  // Playback controls.
  virtual void play() override;
  virtual void pause() override;
  virtual bool supportsSave() const override;
  virtual void seek(double seconds) override;
  virtual void setRate(double) override;
  virtual void setVolume(double) override;
  virtual blink::WebTimeRanges buffered() const override;
  virtual blink::WebTimeRanges seekable() const override;

  virtual void paint(blink::WebCanvas*,
      const blink::WebRect&,
      unsigned char alpha,
      SkXfermode::Mode) override;

  // True if the loaded media has a playable video/audio track.
  virtual bool hasVideo() const override;
  virtual bool hasAudio() const override;

  // Dimension of the video.
  virtual blink::WebSize naturalSize() const override;

  // Getters of playback state.
  virtual bool paused() const override;
  virtual bool seeking() const override;
  virtual double duration() const override;
  virtual double currentTime() const override;

  // Internal states of loading and network.
  virtual NetworkState networkState() const override;
  virtual ReadyState readyState() const override;

  virtual bool didLoadingProgress() override;

  virtual bool hasSingleSecurityOrigin() const override;
  virtual bool didPassCORSAccessCheck() const override;

  virtual double mediaTimeForTimeValue(double timeValue) const override;

  virtual unsigned decodedFrameCount() const override { return 0;};
  virtual unsigned droppedFrameCount() const override{ return 0;};;
  virtual unsigned audioDecodedByteCount() const override{ return 0;};;
  virtual unsigned videoDecodedByteCount() const override{ return 0;};;

  // cc::VideoFrameProvider implementation.
  virtual void SetVideoFrameProviderClient(
      cc::VideoFrameProvider::Client* client) override;
  virtual scoped_refptr<media::VideoFrame> GetCurrentFrame() override;
  virtual void PutCurrentFrame(
      const scoped_refptr<media::VideoFrame>& frame) override {};

  // Method inherited from DestructionObserver.
  virtual void WillDestroyCurrentMessageLoop() override {};

  void SetReadyState(WebMediaPlayer::ReadyState state);
  void SetNetworkState(WebMediaPlayer::NetworkState state);

  void OnNewFrameAvailable(base::SharedMemoryHandle foreign_memory_handle,
      uint32 length, base::TimeDelta timestamp);

#if defined(TIZEN_MULTIMEDIA_PIXMAP_SUPPORT)
  void OnPlatformSurfaceUpdated(int pixmap_id, base::TimeDelta timestamp);
#endif

  void OnMediaDataChange(int format, int height, int width, int media);
  void OnDurationChange(double duration);
  void OnTimeUpdate(double current_time);
  void OnBufferUpdate(
      std::vector<media::MediaPlayerTizen::TimeRanges> buffer_range);
  void OnTimeChanged();
  void OnPauseStateChange(bool state);
  void OnSeekStateChange(bool state);
  void OnRequestSeek(double seek_time);

  void OnMediaSourceOpened(blink::WebMediaSource* web_media_source);
  void OnDemuxerSeekDone();

 void RequestPause();

 private:
  void OnNaturalSizeChanged(gfx::Size size);
  void OnOpacityChanged(bool opaque);

  // Returns the current video frame from |compositor_|. Blocks until the
  // compositor can return the frame.
  scoped_refptr<media::VideoFrame> GetCurrentFrameFromCompositor();

  // Called whenever there is new frame to be painted.
  void FrameReady(const scoped_refptr<media::VideoFrame>& frame);

  blink::WebFrame* frame_;

  blink::WebMediaPlayer::NetworkState network_state_;
  blink::WebMediaPlayer::ReadyState ready_state_;

  // Message loops for posting tasks on Chrome's main thread. Also used
  // for DCHECKs so methods calls won't execute in the wrong thread.
  const scoped_refptr<base::MessageLoopProxy> main_loop_;
  scoped_refptr<base::SingleThreadTaskRunner> media_task_runner_;

  // Manager for managing this object and for delegating method calls on
  // Render Thread.
  content::RendererMediaPlayerManagerTizen* manager_;

  blink::WebMediaPlayerClient* client_;

  scoped_refptr<media::MediaLog> media_log_;

  base::WeakPtr<media::WebMediaPlayerDelegate> delegate_;

  // The compositor layer for displaying the video content when using
  // composited playback.
  scoped_ptr<cc_blink::WebLayerImpl> video_weblayer_;


 // Video rendering members.
  scoped_refptr<base::SingleThreadTaskRunner> compositor_task_runner_;
  media::VideoFrameCompositor* compositor_;
  media::SkCanvasVideoRenderer skcanvas_video_renderer_;

  base::WeakPtrFactory<WebMediaPlayerTizen> weak_factory_;
  scoped_ptr<content::MediaSourceDelegateTizen,
      content::MediaSourceDelegateTizen::Destroyer> media_source_delegate_;
  MediaPlayerHostMsg_Initialize_Type player_type_;

  // Player ID assigned by the |manager_|.
  int player_id_;

  uint32 gst_video_format_;
  int gst_width_;
  int gst_height_;

  bool FrameAvailable_;
  bool audio_;
  bool video_;

  double current_time_;
  double duration_;
  bool is_paused_;

  bool is_seeking_;
  double seek_time_;
  bool pending_seek_;
  double pending_seek_time_;

  // Whether the video is known to be opaque or not.
  bool opaque_;

  gfx::Size natural_size_;
  blink::WebTimeRanges buffered_;
  mutable bool did_loading_progress_;
  double volume_;

  // Factories for supporting video accelerators. May be null.
  scoped_refptr<media::GpuVideoAcceleratorFactories> gpu_factories_;

  DISALLOW_COPY_AND_ASSIGN(WebMediaPlayerTizen);
};

}  // namespace media

#endif  // CONTENT_RENDERER_MEDIA_TIZEN_WEBMEDIAPLAYER_TIZEN_H_
