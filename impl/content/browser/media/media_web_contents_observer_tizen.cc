// Copyright 2014 Samsung Electronics Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/browser/media/media_web_contents_observer_tizen.h"

#include "content/browser/media/tizen/browser_media_player_manager_tizen.h"
#include "content/browser/renderer_host/render_process_host_impl.h"
#include "content/common/media/tizen/media_player_messages_tizen.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/web_contents.h"
#include "ipc/ipc_message_macros.h"

namespace content {

MediaWebContentsObserverTizen::MediaWebContentsObserverTizen(
    RenderViewHost* render_view_host)
    : WebContentsObserver(WebContents::FromRenderViewHost(render_view_host)) {
}

MediaWebContentsObserverTizen::~MediaWebContentsObserverTizen() {
}

void MediaWebContentsObserverTizen::RenderFrameDeleted(
    RenderFrameHost* render_frame_host) {
  uintptr_t key = reinterpret_cast<uintptr_t>(render_frame_host);
  media_player_managers_.erase(key);
}


bool MediaWebContentsObserverTizen::OnMessageReceived(const IPC::Message& msg,
                                                      RenderFrameHost* render_frame_host) {
  return OnMediaPlayerMessageReceived(msg, render_frame_host);
}

bool MediaWebContentsObserverTizen::OnMediaPlayerMessageReceived(
    const IPC::Message& msg,
    RenderFrameHost* render_frame_host) {
  bool handled = true;

  IPC_BEGIN_MESSAGE_MAP(MediaWebContentsObserverTizen, msg)
    IPC_MESSAGE_FORWARD(MediaPlayerGstHostMsg_Init,
                        GetMediaPlayerManager(render_frame_host),
                        BrowserMediaPlayerManagerTizen::OnInitialize)

    IPC_MESSAGE_FORWARD(MediaPlayerGstHostMsg_DeInit,
                        GetMediaPlayerManager(render_frame_host),
                        BrowserMediaPlayerManagerTizen::OnDestroy)

    IPC_MESSAGE_FORWARD(MediaPlayerGstHostMsg_Play,
                        GetMediaPlayerManager(render_frame_host),
                        BrowserMediaPlayerManagerTizen::OnPlay)

    IPC_MESSAGE_FORWARD(MediaPlayerGstHostMsg_Pause,
                        GetMediaPlayerManager(render_frame_host),
                        BrowserMediaPlayerManagerTizen::OnPause)

    IPC_MESSAGE_FORWARD(MediaPlayerGstHostMsg_SetVolume,
                        GetMediaPlayerManager(render_frame_host),
                        BrowserMediaPlayerManagerTizen::OnSetVolume)

    IPC_MESSAGE_FORWARD(MediaPlayerGstHostMsg_SetRate,
                        GetMediaPlayerManager(render_frame_host),
                        BrowserMediaPlayerManagerTizen::OnSetRate)

    IPC_MESSAGE_FORWARD(MediaPlayerGstHostMsg_Seek,
                        GetMediaPlayerManager(render_frame_host),
                        BrowserMediaPlayerManagerTizen::OnSeek)

    IPC_MESSAGE_UNHANDLED(handled = false)

  IPC_END_MESSAGE_MAP()
  return handled;
}

BrowserMediaPlayerManagerTizen*
    MediaWebContentsObserverTizen::GetMediaPlayerManager(
        RenderFrameHost* render_frame_host) {
  uintptr_t key = reinterpret_cast<uintptr_t>(render_frame_host);
  if (!media_player_managers_.contains(key)) {
    media_player_managers_.set(
        key,
        make_scoped_ptr(
            BrowserMediaPlayerManagerTizen::Create(render_frame_host)));
  }
  return media_player_managers_.get(key);
}

}  // namespace content
