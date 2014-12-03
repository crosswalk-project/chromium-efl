// Copyright 2014 Samsung Electronics Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_TIZEN_MEDIA_PLAYER_BRIDGE_CAPI_H_
#define MEDIA_TIZEN_MEDIA_PLAYER_BRIDGE_CAPI_H_

#include <player.h>
#include <player_product.h>

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

class MEDIA_EXPORT MediaPlayerBridgeCapi
    : public MediaPlayerTizen {
 public:
  MediaPlayerBridgeCapi(int player_id,
      const GURL& url,
      double volume,
      MediaPlayerManager* manager);
  virtual ~MediaPlayerBridgeCapi();

  // MediaPlayerTizen implementation.
  virtual void Destroy() override;
  virtual void Play() override;
  virtual void Pause(bool is_media_related_action) override;
  virtual void SetRate(double rate) override;
  virtual void Seek(const double time) override;
  virtual void SetVolume(double volume) override;
  virtual double GetCurrentTime() override;

  int GetSurfaceID() const;
  void ExecuteDelayedPlayerState();
  void HandleBufferingStatus(int percent);
  void HandleError(int ret, char const* from);
  void PlatformSurfaceUpdated();
  void PlaybackCompleteUpdate();
  void SeekCompleteUpdate();
  void SetPixmap();

 protected:
  virtual void Release();

 private:
  // |duration_update_timer_| related
  void OnCurrentTimeUpdateTimerFired();
  void StartCurrentTimeUpdateTimer();
  void StopCurrentTimeUpdateTimer();

  // |buffering_update_timer_| related
  void OnBufferingUpdateTimerFired();
  void StartBufferingUpdateTimer();
  void StopBufferingUpdateTimer();

  void OnTimeChanged();
  void UpdateMediaType();
  void UpdateSeekState(bool state);
  void UpdateDuration();

  player_state_e GetPlayerState();

 private:
  int pixmap_id_;
  scoped_refptr<gfx::EflPixmap> efl_pixmap_;
  Ecore_X_Damage m_damage;
  Ecore_Event_Handler* m_damageHandler;

  const scoped_refptr<base::MessageLoopProxy> main_loop_;

  player_h player_;
  GURL url_;

  int media_type_;
  int player_width_;
  int player_height_;
  unsigned int video_format_;

  bool is_download_finished_;
  bool is_end_reached_;
  bool is_file_url_;
  bool is_paused_;
  bool is_pixmap_used_;
  bool is_seeking_;

  double duration_;
  double seek_duration_;
  double playback_rate_;

  int delayed_player_state_;
  const static int DELAYED_PLAYER_STATE_PLAY  = 1;
  const static int DELAYED_PLAYER_STATE_PAUSE = 2;

  base::SharedMemory shared_memory;
  uint32 shared_memory_size;
  base::SharedMemoryHandle foreign_memory_handle;

  base::RepeatingTimer<MediaPlayerBridgeCapi> current_time_update_timer_;
  base::RepeatingTimer<MediaPlayerBridgeCapi> buffering_update_timer_;

  DISALLOW_COPY_AND_ASSIGN(MediaPlayerBridgeCapi);
};

}  // namespace media

#endif  // MEDIA_TIZEN_MEDIA_PLAYER_BRIDGE_CAPI_H_

