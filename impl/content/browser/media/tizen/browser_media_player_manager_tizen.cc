// Copyright 2014 Samsung Electronics Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/browser/media/tizen/browser_media_player_manager_tizen.h"

#include "base/memory/shared_memory.h"
#include "content/browser/media/tizen/browser_demuxer_tizen.h"
#include "content/browser/renderer_host/render_process_host_impl.h"
#include "content/common/media/tizen/media_player_messages_tizen.h"
#include "content/public/browser/web_contents.h"
#include "ipc/ipc_channel_proxy.h"
#include "ipc/ipc_logging.h"
#include "media/base/tizen/media_source_player_gstreamer.h"

#if defined(TIZEN_CAPI_PLAYER_SUPPORT) && defined(TIZEN_MULTIMEDIA_PIXMAP_SUPPORT)
#include "media/base/tizen/media_player_bridge_capi.h"
#else
#include "media/base/tizen/media_player_bridge_gstreamer.h"
#endif

namespace content {

BrowserMediaPlayerManagerTizen* BrowserMediaPlayerManagerTizen::Create(
    RenderFrameHost* rfh) {
  return new BrowserMediaPlayerManagerTizen(rfh);
}

BrowserMediaPlayerManagerTizen::BrowserMediaPlayerManagerTizen(
    RenderFrameHost* render_frame_host)
    : render_frame_host_(render_frame_host),
      web_contents_(WebContents::FromRenderFrameHost(render_frame_host)),
      weak_ptr_factory_(this) {
}

BrowserMediaPlayerManagerTizen::~BrowserMediaPlayerManagerTizen() {
  for (ScopedVector<media::MediaPlayerTizen>::iterator it = players_.begin();
      it != players_.end(); ++it) {
    (*it)->Destroy();
  }
  players_.weak_clear();
}

media::MediaPlayerTizen* BrowserMediaPlayerManagerTizen::GetPlayer(
    int player_id) {
  for (ScopedVector<media::MediaPlayerTizen>::iterator it = players_.begin();
      it != players_.end(); ++it) {
    if ((*it)->GetPlayerId() == player_id)
      return *it;
  }
  return NULL;
}

void BrowserMediaPlayerManagerTizen::OnNewFrameAvailable(
    int player_id,
    base::SharedMemoryHandle foreign_memory_handle,
    uint32 length, base::TimeDelta timestamp) {
  Send(new MediaPlayerGstMsg_NewFrameAvailable(
      GetRoutingID(), player_id, foreign_memory_handle, length, timestamp));
}

#if defined(TIZEN_MULTIMEDIA_PIXMAP_SUPPORT)
void BrowserMediaPlayerManagerTizen::OnPlatformSurfaceUpdated(
    int player_id,
    int pixmap_id,
    base::TimeDelta timestamp) {
  Send(new MediaPlayerGstMsg_PlatformSurfaceUpdated(
      GetRoutingID(), player_id, pixmap_id, timestamp));
}
#endif

void BrowserMediaPlayerManagerTizen::OnTimeChanged(int player_id) {
  Send(new MediaPlayerGstMsg_TimeChanged(GetRoutingID(), player_id));
}

void BrowserMediaPlayerManagerTizen::OnPauseStateChange(
    int player_id,
    bool state) {
  Send(new MediaPlayerGstMsg_OnPauseStateChange(
      GetRoutingID(), player_id, state));
}

void BrowserMediaPlayerManagerTizen::OnSeekStateChange(
    int player_id,
    bool state) {
  Send(new MediaPlayerGstMsg_OnSeekStateChange(
    GetRoutingID(), player_id, state));
}

void BrowserMediaPlayerManagerTizen::OnRequestSeek(
  int player_id,
  double seek_time) {
  // To handle internal seek.
  Send(new MediaPlayerGstMsg_SeekRequest(
      GetRoutingID(), player_id, seek_time));
}

void BrowserMediaPlayerManagerTizen::OnTimeUpdate(
    int player_id,
    double current_time) {
  Send(new MediaPlayerGstMsg_TimeUpdate(
      GetRoutingID(), player_id, current_time));
}

void BrowserMediaPlayerManagerTizen::OnBufferUpdate(
    int player_id,
    std::vector<media::MediaPlayerTizen::TimeRanges> buffer_range) {
  Send(new MediaPlayerGstMsg_BufferUpdate(
      GetRoutingID(), player_id, buffer_range));
}

void BrowserMediaPlayerManagerTizen::OnDurationChange(
    int player_id,
    double duration) {
  Send(new MediaPlayerGstMsg_DurationChanged(
      GetRoutingID(), player_id, duration));
}

void BrowserMediaPlayerManagerTizen::OnReadyStateChange(
    int player_id,
    media::MediaPlayerTizen::ReadyState state) {
  Send(new MediaPlayerGstMsg_ReadyStateChange(
      GetRoutingID(), player_id, state));
}

void BrowserMediaPlayerManagerTizen::OnNetworkStateChange(
    int player_id,
    media::MediaPlayerTizen::NetworkState state) {
  Send(new MediaPlayerGstMsg_NetworkStateChange(
    GetRoutingID(), player_id, state));
}

void BrowserMediaPlayerManagerTizen::OnMediaDataChange(
    int player_id,
    int format,
    int height,
    int width,
    int media) {
  Send(new MediaPlayerGstMsg_MediaDataChanged(
      GetRoutingID(), player_id, format, height, width, media));
}

int BrowserMediaPlayerManagerTizen::GetRoutingID() {
  if (!render_frame_host_)
    return 0;
  return render_frame_host_->GetRoutingID();
}

bool BrowserMediaPlayerManagerTizen::Send(IPC::Message* msg) {
  if (!render_frame_host_)
    return false;
  return render_frame_host_->Send(msg);
}

void BrowserMediaPlayerManagerTizen::OnInitialize(
    int player_id,
    MediaPlayerHostMsg_Initialize_Type type,
    const GURL& url,
    double volume,
    int demuxer_client_id) {
  RemovePlayer(player_id);

  if (type == MEDIA_PLAYER_TYPE_URL) {

#if defined(TIZEN_CAPI_PLAYER_SUPPORT) && defined(TIZEN_MULTIMEDIA_PIXMAP_SUPPORT)
    LOG(INFO) << "MediaElement is using |CAPI| to play media";
    AddPlayer(new media::MediaPlayerBridgeCapi(player_id, url, volume, this));
#else
    LOG(INFO) << "MediaElement is using |Gstreamer| to play media";
    AddPlayer(
        new media::MediaPlayerBridgeGstreamer(player_id, url, volume, this));
#endif

  } else if (type == MEDIA_PLAYER_TYPE_MEDIA_SOURCE) {
    RenderProcessHostImpl* host = static_cast<RenderProcessHostImpl*>(
        web_contents()->GetRenderProcessHost());
    AddPlayer(new media::MediaSourcePlayerGstreamer(
        player_id,
        host->browser_demuxer_tizen()->CreateDemuxer(demuxer_client_id),
        this));
  } else {
    LOG(ERROR) << __FUNCTION__ << " Load type is wrong!";
  }
}

void BrowserMediaPlayerManagerTizen::OnDestroy(int player_id) {
  VLOG(1) << __FUNCTION__ << " " << player_id;
  RemovePlayer(player_id);
}

void BrowserMediaPlayerManagerTizen::OnPlay(int player_id) {
  media::MediaPlayerTizen* player = GetPlayer(player_id);
  if (player)
    player->Play();
}

void BrowserMediaPlayerManagerTizen::OnPause(int player_id) {
  media::MediaPlayerTizen* player = GetPlayer(player_id);
  if (player)
    player->Pause(false);
}

void BrowserMediaPlayerManagerTizen::OnSetVolume(
    int player_id,
    double volume) {
  media::MediaPlayerTizen* player = GetPlayer(player_id);
  if (player)
    player->SetVolume(volume);
}

void BrowserMediaPlayerManagerTizen::OnSetRate(int player_id, double rate) {
  media::MediaPlayerTizen* player = GetPlayer(player_id);
  if (player)
    player->SetRate(rate);
}

void BrowserMediaPlayerManagerTizen::OnSeek(int player_id, double time) {
  media::MediaPlayerTizen* player = GetPlayer(player_id);
  if (player)
    player->Seek(time);
}

void BrowserMediaPlayerManagerTizen::AddPlayer(
    media::MediaPlayerTizen* player) {
  DCHECK(!GetPlayer(player->GetPlayerId()));
  VLOG(1) << "BrowserMediaPlayerManagerTizen::" << __FUNCTION__
          << " Player-Id : " << player->GetPlayerId();
  players_.push_back(player);
}

void BrowserMediaPlayerManagerTizen::RemovePlayer(int player_id) {
  for (ScopedVector<media::MediaPlayerTizen>::iterator it = players_.begin();
      it != players_.end(); ++it) {
    media::MediaPlayerTizen* player = *it;
    if (player->GetPlayerId() == player_id) {
      VLOG(1) << "BrowserMediaPlayerManagerTizen::" << __FUNCTION__
              << " Player-Id : " << player->GetPlayerId();
      players_.weak_erase(it);
      player->Destroy();
      break;
    }
  }
}

} // namespace content

