// Copyright 2014 Samsung Electronics Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_TIZEN_MEDIA_PLAYER_TIZEN_MANAGER_H_
#define MEDIA_BASE_TIZEN_MEDIA_PLAYER_TIZEN_MANAGER_H_

#include <vector>

#include "base/memory/shared_memory.h"
#include "media/base/tizen/media_player_tizen.h"

namespace media {

class MediaPlayerTizen;

class MEDIA_EXPORT MediaPlayerManager {
 public:
  virtual ~MediaPlayerManager() { }
  virtual MediaPlayerTizen* GetPlayer(int player_id) = 0;

  virtual void OnTimeChanged(int player_id) = 0;
  virtual void OnTimeUpdate(int player_id,  double current_time) = 0;
  virtual void OnRequestSeek(int player_id,  double seek_time) = 0;
  virtual void OnPauseStateChange(int player_id,  bool state) = 0;
  virtual void OnSeekStateChange(int player_id,  bool state) = 0;
  virtual void OnBufferUpdate(
      int player_id,
      std::vector<media::MediaPlayerTizen::TimeRanges> buffer_range) = 0;
  virtual void OnDurationChange(int player_id,  double duration) = 0;
  virtual void OnReadyStateChange(int player_id,
      MediaPlayerTizen::ReadyState state) = 0;
  virtual void OnNetworkStateChange(
      int player_id,
      MediaPlayerTizen::NetworkState state) = 0;
  virtual void OnMediaDataChange(
      int player_id,
      int format,
      int height,
      int width,
      int media) = 0;
  virtual void OnNewFrameAvailable(
      int player_id,
      base::SharedMemoryHandle handle,
      uint32 length,
      base::TimeDelta timestamp) = 0;
#if defined(TIZEN_MULTIMEDIA_PIXMAP_SUPPORT)
  virtual void OnPlatformSurfaceUpdated(
      int player_id,
      int pixmap_id,
      base::TimeDelta timestamp) = 0;
#endif
};

}  // namespace media

#endif  // MEDIA_BASE_TIZEN_MEDIA_PLAYER_TIZEN_MANAGER_H_