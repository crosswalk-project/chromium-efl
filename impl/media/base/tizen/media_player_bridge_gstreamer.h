// Copyright 2014 Samsung Electronics Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_TIZEN_MEDIA_PLAYER_BRIDGE_GSTREMEAR_H_
#define MEDIA_TIZEN_MEDIA_PLAYER_BRIDGE_GSTREMEAR_H_

#include <gst/gst.h>
#include <gst/pbutils/install-plugins.h>
#include <gst/pbutils/missing-plugins.h>

#if defined(TIZEN_MULTIMEDIA_PIXMAP_SUPPORT)
#include <Ecore.h>
#include <Ecore_X.h>
#endif

#include "base/memory/scoped_ptr.h"
#include "base/memory/weak_ptr.h"
#include "base/timer/timer.h"
#include "content/public/browser/browser_message_filter.h"
#include "media/base/ranges.h"
#include "media/base/tizen/media_player_tizen.h"
#include "media/base/video_frame.h"

#if defined(TIZEN_MULTIMEDIA_PIXMAP_SUPPORT)
#include "ui/gl/efl_pixmap.h"
#endif

namespace media {

class MEDIA_EXPORT MediaPlayerBridgeGstreamer
    : public MediaPlayerTizen {
 public:
  MediaPlayerBridgeGstreamer(
      int player_id,
      const GURL& url,
      double volume,
      MediaPlayerManager* manager);
  virtual ~MediaPlayerBridgeGstreamer();

  // MediaPlayerTizen implementation.
  virtual void Play() override;
  virtual void Pause(bool is_media_related_action) override;
  virtual void SetRate(double rate) override;
  virtual void Seek(const double time) override;
  virtual void SetVolume(double volume) override;
  virtual double GetCurrentTime() override;
  virtual void Destroy() override;

  // Error handling API
  void HandleError(media::MediaPlayerTizen::NetworkState state);

  void HandleMessage(GstMessage* message);
  void HandlePluginInstallerResult(GstInstallPluginsReturn result);
  GstBuffer* PullBuffer();
  void BufferReady(const GstBuffer* buffer);
  void PrerollComplete();

#if defined(TIZEN_MULTIMEDIA_PIXMAP_SUPPORT)
  void PlatformSurfaceUpdated();
  int GetSurfaceID() const;
#endif

 protected:
  virtual void Release();

 private:
  // |duration_update_timer_| related
  void OnCurrentTimeUpdateTimerFired();
  void StartCurrentTimeUpdateTimer();
  void StopCurrentTimeUpdateTimer();

  // |Buffering_update_timer_| related
  void OnBufferingUpdateTimerFired();
  void StartBufferingUpdateTimer();
  void StopBufferingUpdateTimer();

  void UpdateStates();
  void OnUpdateStates();
  void UpdateDuration();
  void UpdateMediaType();
  void UpdateSeekState(bool state);

  void OnPlaybackComplete();
  void OnTimeChanged();

  void FrameReady(const scoped_refptr<media::VideoFrame>& frame);
  void GetFrameDetails();
  void ProcessBufferingStats(GstMessage* message);
  void GetBufferedTimeRanges();
  bool SeekTo(gint64 position, float rate, GstSeekFlags seekType);

  GstElement* GetVideoSink();
  void PrepareForVideoFrame();

#if defined(TIZEN_MULTIMEDIA_PIXMAP_SUPPORT)
  void PrepareForVideoSink();
  void XWindowIdPrepared(GstMessage*);
  bool IsXWindowHadleSet(){return pixmap_id_ ? true : false ;}
  void SetPixmap();
#endif

 private:
  const scoped_refptr<base::MessageLoopProxy> main_loop_;
  GstElement* gst_playbin_;
  GstElement* gst_appsink_;
  GURL url_;
  double volume_;
  gint gst_width_;
  gint gst_height_;

  bool is_prerolled_;
  bool is_paused_;
  double duration_;
  double playback_rate_;
  int buffered_;
  guint32 video_format_;

  int media_type_;
  bool is_live_stream_;
  bool is_file_url_;
  bool is_end_reached_ ;
  bool is_seeking_ ;
  bool is_seek_pending_ ;
  double seek_duration_;

  bool error_occured_;
  bool missing_plugins_;
  bool is_pixmap_used_;

#if defined(TIZEN_MULTIMEDIA_PIXMAP_SUPPORT)
  int pixmap_id_;
  scoped_refptr<gfx::EflPixmap> efl_pixmap_;
  Ecore_X_Damage m_damage;
  Ecore_Event_Handler* m_damageHandler;
#endif
  uint bufsize_sn12_;

  base::SharedMemory shared_memory;
  uint32 shared_memory_size;
  base::SharedMemoryHandle foreign_memory_handle;

  base::RepeatingTimer<MediaPlayerBridgeGstreamer> current_time_update_timer_;
  base::RepeatingTimer<MediaPlayerBridgeGstreamer> buffering_update_timer_;

  DISALLOW_COPY_AND_ASSIGN(MediaPlayerBridgeGstreamer);
};

}  // namespace media

#endif  // MEDIA_TIZEN_MEDIA_PLAYER_BRIDGE_GSTREMEAR_H_
