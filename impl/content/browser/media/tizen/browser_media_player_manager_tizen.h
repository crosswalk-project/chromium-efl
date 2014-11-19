// Copyright 2014 Samsung Electronics Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_BROWSER_MEDIA_TIZEN_BROWSER_MEDIA_PLAYER_MANAGER_TIZEN_H_
#define CONTENT_BROWSER_MEDIA_TIZEN_BROWSER_MEDIA_PLAYER_MANAGER_TIZEN_H_

#include "base/memory/scoped_vector.h"
#include "content/public/browser/browser_message_filter.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/web_contents_observer.h"
#include "media/base/tizen/media_player_manager_tizen.h"

namespace content {

// This class manages all the MediaPlayerTizen objects. It receives
// control operations from the the render process, and forwards
// them to corresponding MediaPlayerTizen object. Callbacks from
// MediaPlayerTizen objects are converted to IPCs and then sent to the
// render process.
class CONTENT_EXPORT BrowserMediaPlayerManagerTizen
    : public media::MediaPlayerManager {
 public:
  // Returns a new instance using the registered factory if available.
  static BrowserMediaPlayerManagerTizen* Create( RenderFrameHost* efh);
  virtual ~BrowserMediaPlayerManagerTizen();

  // media::MediaPlayerManager implementation.
  virtual media::MediaPlayerTizen* GetPlayer(int player_id) override;
  virtual void OnTimeChanged(int player_id) override;
  virtual void OnTimeUpdate(int player_id, double current_time) override;
  virtual void OnPauseStateChange(int player_id, bool state) override;
  virtual void OnSeekStateChange(int player_id, bool state) override;
  virtual void OnRequestSeek(int player_id, double seek_time) override;
  virtual void OnBufferUpdate(
      int player_id,
      std::vector<media::MediaPlayerTizen::TimeRanges> buffer_range) override;
  virtual void OnDurationChange(int player_id, double duration) override;
  virtual void OnReadyStateChange(
      int player_id,
      media::MediaPlayerTizen::ReadyState state) override;
  virtual void OnNetworkStateChange(
      int player_id,
      media::MediaPlayerTizen::NetworkState state) override;
  virtual void OnMediaDataChange(
      int player_id,
      int format,
      int height,
      int width,
      int media) override;
  virtual void OnNewFrameAvailable(
      int player_id,
      base::SharedMemoryHandle foreign_memory_handle,
      uint32 length, base::TimeDelta timestamp) override;

#if defined(TIZEN_MULTIMEDIA_PIXMAP_SUPPORT)
  virtual void OnPlatformSurfaceUpdated(
      int player_id,
      int pixmap_id,
      base::TimeDelta timestamp) override;
#endif

  // Helper function to handle IPC from RenderMediaPlayerMangaerTizen.
  virtual void OnPlay(int player_id);
  virtual void OnPause(int player_id);
  virtual void OnSetVolume(int player_id, double volume);
  virtual void OnSetRate(int player_id, double rate);
  virtual void OnInitialize(
      int player_id,
      MediaPlayerHostMsg_Initialize_Type type,
      const GURL& url,
      double volume,
      int demuxer_client_id);
  virtual void OnDestroy(int player_id);
  virtual void OnSeek(int player_id, double time);

 protected:
  // Clients must use Create() or subclass constructor.
  explicit BrowserMediaPlayerManagerTizen(RenderFrameHost* render_frame_host);

  void AddPlayer(media::MediaPlayerTizen* player);
  void RemovePlayer(int player_id);

  // Helper function to send messages to RenderFrameObserver.
  bool Send(IPC::Message* msg);

  int GetRoutingID();
  WebContents* web_contents() const { return web_contents_; }

 private:
  // An array of managed players.
  ScopedVector<media::MediaPlayerTizen> players_;
  RenderFrameHost* render_frame_host_;
  WebContents* const web_contents_;
  base::WeakPtrFactory<BrowserMediaPlayerManagerTizen> weak_ptr_factory_;

  DISALLOW_COPY_AND_ASSIGN(BrowserMediaPlayerManagerTizen);
};

}  // namespace content

#endif  // CONTENT_BROWSER_MEDIA_TIZEN_BROWSER_MEDIA_PLAYER_MANAGER_TIZEN_H_
