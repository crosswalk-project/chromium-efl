// Copyright 2014 Samsung Electronics Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_TIZEN_MEDIA_PLAYER_TIZEN_H_
#define MEDIA_TIZEN_MEDIA_PLAYER_TIZEN_H_

#include <string>

#include "base/basictypes.h"
#include "base/callback.h"
#include "base/time/time.h"
#include "content/common/media/tizen/media_player_messages_enums_tizen.h"
#include "media/base/media_export.h"
#include "url/gurl.h"

namespace media {

class MediaDrmBridge;
class MediaPlayerManager;

// Error types for MediaErrorCB.
enum MediaErrorType {
  MEDIA_ERROR_FORMAT,
  MEDIA_ERROR_DECODE,
  MEDIA_ERROR_NOT_VALID_FOR_PROGRESSIVE_PLAYBACK,
  MEDIA_ERROR_INVALID_CODE,
};

// media type masks
const int MEDIA_AUDIO_MASK = 0x02;
const int MEDIA_VIDEO_MASK = 0x01;

class MEDIA_EXPORT MediaPlayerTizen{
 public:
  // FIXME: Remove these enum and use webkit's
  enum ReadyState {
    ReadyStateHaveNothing,
    ReadyStateHaveMetadata,
    ReadyStateHaveCurrentData,
    ReadyStateHaveFutureData,
    ReadyStateHaveEnoughData,
  };

  enum NetworkState {
    NetworkStateEmpty,
    NetworkStateIdle,
    NetworkStateLoading,
    NetworkStateLoaded,
    NetworkStateFormatError,
    NetworkStateNetworkError,
    NetworkStateDecodeError,
  };

  typedef struct range {
    int64 start;
    int64 end;
  } TimeRanges;

  MediaPlayerTizen() { }
  virtual ~MediaPlayerTizen() { }

  // Start playing the media.
  virtual void Play() = 0;

  // To Pause from HTMLMediaElement, pass |false| as argument.
  // Pass |true| as argument when buffer is low in case of MediaSource.
  virtual void Pause(bool is_media_related_action) = 0;

  virtual void SetRate(double rate) = 0;

  // Seek to a particular position, based on renderer signaling actual seek
  // with MediaPlayerHostMsg_Seek. If eventual success, OnSeekComplete() will be
  // called.
  virtual void Seek(const double time) = 0;

  // Set the player volume.
  virtual void SetVolume(double volume) = 0;

  // Get the media information from the player.
  virtual double GetCurrentTime() = 0;

  int GetPlayerId() { return player_id_; }

  bool IsPlayerDestructing() { return destructing_; }

  // Destroy this object when all messages for it are delivered
  virtual void Destroy() = 0;

 protected:
  // Release the player resources.
  virtual void Release() = 0;

  MediaPlayerTizen(int player_id, MediaPlayerManager* manager)
    : destructing_(false), player_id_(player_id), manager_(manager) { }

  MediaPlayerManager* manager() { return manager_; }

  bool destructing_;

 private:
  // Player ID assigned to this player.
  int player_id_;

  // Resource manager for all the media players.
  MediaPlayerManager* manager_;

  DISALLOW_COPY_AND_ASSIGN(MediaPlayerTizen);
};

}  // namespace media

#endif // MEDIA_TIZEN_MEDIA_PLAYER_TIZEN_H_
