// Copyright 2014 Samsung Electronics Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/renderer/media/tizen/renderer_media_player_manager_tizen.h"

#include "base/bind.h"
#include "base/message_loop/message_loop.h"
#include "content/common/media/tizen/media_player_messages_tizen.h"
#include "ui/gfx/rect_f.h"

namespace content {

RendererMediaPlayerManagerTizen::RendererMediaPlayerManagerTizen(
    RenderFrame* render_frame)
    : RenderFrameObserver(render_frame),
      next_media_player_id_(0) {
}

RendererMediaPlayerManagerTizen::~RendererMediaPlayerManagerTizen() {
  DCHECK(media_players_.empty())
      << "RendererMediaPlayerManagerTizen is owned by RenderFrameImpl and is "
         "destroyed only after all media players are destroyed.";
}

void RendererMediaPlayerManagerTizen::PausePlayingPlayers() {
  std::map<int, media::WebMediaPlayerTizen*>::iterator player_it;
  for (player_it = media_players_.begin();player_it != media_players_.end();) {
    media::WebMediaPlayerTizen* player = player_it->second;
    // Element pointed by the iterator can get deleted during the function call
    // "player->RequestPause()", hence increment the iterator beforehand
    ++player_it;
    if (player && !player->paused() && player->hasVideo())
      player->RequestPause();
  }
}

bool RendererMediaPlayerManagerTizen::OnMessageReceived(
    const IPC::Message& message) {
  bool handled = true;
  IPC_BEGIN_MESSAGE_MAP(RendererMediaPlayerManagerTizen, message)
    IPC_MESSAGE_HANDLER(MediaPlayerGstMsg_MediaDataChanged, OnMediaDataChange)
    IPC_MESSAGE_HANDLER(MediaPlayerGstMsg_DurationChanged, OnDurationChange)
    IPC_MESSAGE_HANDLER(MediaPlayerGstMsg_TimeUpdate, OnTimeUpdate)
    IPC_MESSAGE_HANDLER(MediaPlayerGstMsg_BufferUpdate, OnBufferUpdate)
    IPC_MESSAGE_HANDLER(MediaPlayerGstMsg_ReadyStateChange, OnReadyStateChange)
    IPC_MESSAGE_HANDLER(MediaPlayerGstMsg_NetworkStateChange,
                        OnNetworkStateChange)
    IPC_MESSAGE_HANDLER(MediaPlayerGstMsg_TimeChanged, OnTimeChanged)
    IPC_MESSAGE_HANDLER(MediaPlayerGstMsg_OnPauseStateChange,
                        OnPauseStateChange)
    IPC_MESSAGE_HANDLER(MediaPlayerGstMsg_OnSeekStateChange, OnSeekStateChange)
    IPC_MESSAGE_HANDLER(MediaPlayerGstMsg_SeekRequest, OnRequestSeek)
    IPC_MESSAGE_HANDLER(MediaPlayerGstMsg_NewFrameAvailable,
                        OnNewFrameAvailable)
#if defined(TIZEN_MULTIMEDIA_PIXMAP_SUPPORT)
    IPC_MESSAGE_HANDLER(MediaPlayerGstMsg_PlatformSurfaceUpdated,
                        OnPlatformSurfaceUpdated)
#endif
  IPC_MESSAGE_UNHANDLED(handled = false)
  IPC_END_MESSAGE_MAP()
  return handled;
}

void RendererMediaPlayerManagerTizen::Initialize(
    int player_id,
    MediaPlayerHostMsg_Initialize_Type type,
    const GURL& url,
    double volume,
    int demuxer_client_id) {
  Send(new MediaPlayerGstHostMsg_Init(
      routing_id(), player_id, type, url, volume, demuxer_client_id));
}

void RendererMediaPlayerManagerTizen::OnMediaDataChange(
    int player_id,
    int format,
    int height,
    int width,
    int media) {
  media::WebMediaPlayerTizen* player = GetMediaPlayer(player_id);
  if (player)
    player->OnMediaDataChange(format, height, width, media);
}

void RendererMediaPlayerManagerTizen::OnDurationChange(
    int player_id,
    double duration) {
  media::WebMediaPlayerTizen* player = GetMediaPlayer(player_id);
  if (player)
    player->OnDurationChange(duration);
}

void RendererMediaPlayerManagerTizen::OnTimeUpdate(
    int player_id,
    double current_time) {
  media::WebMediaPlayerTizen* player = GetMediaPlayer(player_id);
  if (player)
    player->OnTimeUpdate(current_time);
}

void RendererMediaPlayerManagerTizen::OnBufferUpdate(
    int player_id,
    std::vector<media::MediaPlayerTizen::TimeRanges> buffer_range) {
  media::WebMediaPlayerTizen* player = GetMediaPlayer(player_id);
  if (player)
    player->OnBufferUpdate(buffer_range);
}

void RendererMediaPlayerManagerTizen::OnReadyStateChange(
    int player_id,
    media::MediaPlayerTizen::ReadyState state) {
  media::WebMediaPlayerTizen* player = GetMediaPlayer(player_id);
  if (player)
    player->SetReadyState(
        static_cast<blink::WebMediaPlayer::ReadyState>(state));
}

void RendererMediaPlayerManagerTizen::OnNetworkStateChange(
    int player_id,
    media::MediaPlayerTizen::NetworkState state) {
  media::WebMediaPlayerTizen* player = GetMediaPlayer(player_id);
  if (player)
    player->SetNetworkState(
        static_cast<blink::WebMediaPlayer::NetworkState>(state));
}

void RendererMediaPlayerManagerTizen::OnTimeChanged(int player_id) {
  media::WebMediaPlayerTizen* player = GetMediaPlayer(player_id);
  if (player)
    player->OnTimeChanged();
}

void RendererMediaPlayerManagerTizen::OnPauseStateChange(
    int player_id,
    bool state) {
  media::WebMediaPlayerTizen* player = GetMediaPlayer(player_id);
  if (player)
    player->OnPauseStateChange(state);
}

void RendererMediaPlayerManagerTizen::OnSeekStateChange(
    int player_id,
    bool state) {
  media::WebMediaPlayerTizen* player = GetMediaPlayer(player_id);
  if (player)
    player->OnSeekStateChange(state);
}

void RendererMediaPlayerManagerTizen::OnRequestSeek(
    int player_id,
    double seek_time) {
  media::WebMediaPlayerTizen* player = GetMediaPlayer(player_id);
  if (player)
    player->OnRequestSeek(seek_time);
}

void RendererMediaPlayerManagerTizen::OnNewFrameAvailable(
    int player_id,
    base::SharedMemoryHandle foreign_memory_handle,
    uint32 length, base::TimeDelta timestamp) {
  media::WebMediaPlayerTizen* player = GetMediaPlayer(player_id);
  // FIXME: Handle exception for all APIs.
  if (player)
    player->OnNewFrameAvailable(foreign_memory_handle, length, timestamp);
}

#if defined(TIZEN_MULTIMEDIA_PIXMAP_SUPPORT)
void RendererMediaPlayerManagerTizen::OnPlatformSurfaceUpdated(
    int player_id,
    int pixmap_id,
    base::TimeDelta timestamp) {
  media::WebMediaPlayerTizen* player = GetMediaPlayer(player_id);
  if (player)
    player->OnPlatformSurfaceUpdated(pixmap_id, timestamp);
}
#endif

media::WebMediaPlayerTizen* RendererMediaPlayerManagerTizen::GetMediaPlayer(
    int player_id) {
  std::map<int, media::WebMediaPlayerTizen*>::iterator iter =
    media_players_.find(player_id);
  if (iter != media_players_.end())
    return iter->second;

  return NULL;
}

void RendererMediaPlayerManagerTizen::Play(int player_id) {
  Send(new MediaPlayerGstHostMsg_Play(routing_id(), player_id));
}

void RendererMediaPlayerManagerTizen::Pause(
    int player_id,
    bool is_media_related_action) {
  Send(new MediaPlayerGstHostMsg_Pause(routing_id(), player_id));
}

void RendererMediaPlayerManagerTizen::Seek(int player_id, double time) {
  Send(new MediaPlayerGstHostMsg_Seek(routing_id(), player_id, time));
}

void RendererMediaPlayerManagerTizen::SetVolume(int player_id, double volume) {
  Send(new MediaPlayerGstHostMsg_SetVolume(routing_id(), player_id, volume));
}

void RendererMediaPlayerManagerTizen::SetRate(int player_id, double rate) {
  Send(new MediaPlayerGstHostMsg_SetRate(routing_id(), player_id, rate));
}

void RendererMediaPlayerManagerTizen::DestroyPlayer(int player_id) {
  VLOG(1) << "RendererMediaPlayerManagerTizen::" << __FUNCTION__
          << " Plyer-Id = " << player_id;
  Send(new MediaPlayerGstHostMsg_DeInit(routing_id(), player_id));
}

int RendererMediaPlayerManagerTizen::RegisterMediaPlayer(
    media::WebMediaPlayerTizen* player) {
  media_players_[next_media_player_id_] = player;
  return next_media_player_id_++;
}

void RendererMediaPlayerManagerTizen::UnregisterMediaPlayer(int player_id) {
  VLOG(1) << "RendererMediaPlayerManagerTizen::" << __FUNCTION__
          << " Player-Id = " << player_id;
  media_players_.erase(player_id);
}

}  // namespace content
