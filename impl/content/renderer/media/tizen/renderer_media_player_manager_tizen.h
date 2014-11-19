// Copyright 2014 Samsung Electronics Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_RENDERER_MEDIA_TIZEN_RENDERER_MEDIA_PLAYER_TIZEN_MANAGER_H_
#define CONTENT_RENDERER_MEDIA_TIZEN_RENDERER_MEDIA_PLAYER_TIZEN_MANAGER_H_

#include "content/public/renderer/render_frame_observer.h"
#include "media/base/tizen/media_player_tizen.h"
#include "media/base/tizen/webmediaplayer_tizen.h"
#include "url/gurl.h"

namespace media {
class WebMediaPlayerTizen;
}

namespace content {

class RendererMediaPlayerManagerTizen : public RenderFrameObserver {
 public:
  // Constructs a RendererMediaPlayerManagerTizen object for the |render_frame|.
  explicit RendererMediaPlayerManagerTizen(RenderFrame* render_frame);
  virtual ~RendererMediaPlayerManagerTizen();

  // Initializes a MediaPlayerTizen object in browser process.
  void Initialize(
      int player_id,
      MediaPlayerHostMsg_Initialize_Type type,
      const GURL& url,
      double volume,
      int demuxer_client_id);

  // Starts the player.
  void Play(int player_id);

  // Pauses the player.
  // is_media_related_action should be true if this pause is coming from an
  // an action that explicitly pauses the video (user pressing pause, JS, etc.)
  // Otherwise it should be false if Pause is being called due to other reasons
  // (cleanup, freeing resources, etc.)
  void Pause(int player_id, bool is_media_related_action);

  // Performs seek on the player.
  void Seek(int player_id, double time);

  // Sets the player volume.
  void SetVolume(int player_id, double volume);

  // Sets the playback rate.
  void SetRate(int player_id, double rate);

  // Destroys the player in the browser process
  void DestroyPlayer(int player_id);

  // Registers and unregisters a WebMediaPlayerTizen object.
  int RegisterMediaPlayer(media::WebMediaPlayerTizen* player);
  void UnregisterMediaPlayer(int player_id);

  bool OnMessageReceived(const IPC::Message& message) override;

  media::WebMediaPlayerTizen* GetMediaPlayer(int player_id);

  //Pause the playing media players when tab/webpage goes to background
  void PausePlayingPlayers();

 private:
  void OnNewFrameAvailable(
      int player_id,
      base::SharedMemoryHandle foreign_memory_handle,
      uint32 length, base::TimeDelta timestamp);

#if defined(TIZEN_MULTIMEDIA_PIXMAP_SUPPORT)
  void OnPlatformSurfaceUpdated(
      int player_id,
      int pixmap_id,
      base::TimeDelta timestamp);
#endif

  void OnMediaDataChange(
      int player_id,
      int format,
      int height,
      int width,
      int media);
  void OnDurationChange(int player_id, double duration);
  void OnTimeUpdate(int player_id, double current_time);
  void OnBufferUpdate(
      int player_id,
      std::vector<media::MediaPlayerTizen::TimeRanges> buffer_range);
  void OnTimeChanged(int player_id);
  void OnPauseStateChange(int player_id, bool state);
  void OnSeekStateChange(int player_id, bool state);
  void OnRequestSeek(int player_id, double seek_time);
  void OnReadyStateChange(
      int player_id,
      media::MediaPlayerTizen::ReadyState state );
  void OnNetworkStateChange(
      int player_id,
      media::MediaPlayerTizen::NetworkState state );

 private:
  std::map<int, media::WebMediaPlayerTizen*> media_players_;

  int next_media_player_id_;

  DISALLOW_COPY_AND_ASSIGN(RendererMediaPlayerManagerTizen);
};

}  // namespace content

#endif  // CONTENT_RENDERER_MEDIA_TIZEN_RENDERER_MEDIA_PLAYER_TIZEN_MANAGER_H_
