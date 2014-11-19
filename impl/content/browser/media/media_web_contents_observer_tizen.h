// Copyright 2014 Samsung Electronics Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_BROWSER_MEDIA_MEDIA_WEB_CONTENTS_OBSERVER_TIZEN_H_
#define CONTENT_BROWSER_MEDIA_MEDIA_WEB_CONTENTS_OBSERVER_TIZEN_H_

#include "base/compiler_specific.h"
#include "base/containers/scoped_ptr_hash_map.h"
#include "content/common/content_export.h"
#include "content/public/browser/web_contents_observer.h"

namespace content {

class BrowserMediaPlayerManagerTizen;
class RenderViewHost;

// This class manages all RenderFrame based media related managers at the
// browser side. It receives IPC messages from media RenderFrameObservers and
// forwards them to the corresponding managers. The managers are responsible
// for sending IPCs back to the RenderFrameObservers at the render side.
class CONTENT_EXPORT MediaWebContentsObserverTizen
    : public WebContentsObserver {
 public:
  explicit MediaWebContentsObserverTizen(RenderViewHost* render_view_host);
  virtual ~MediaWebContentsObserverTizen();

  // WebContentsObserver implementations.
  virtual void RenderFrameDeleted(RenderFrameHost* render_frame_host) override;
  virtual bool OnMessageReceived(const IPC::Message& message,
                                 RenderFrameHost* render_frame_host) override;

  // Helper functions to handle media player IPC messages. Returns whether the
  // |message| is handled in the function.
  bool OnMediaPlayerMessageReceived(const IPC::Message& message,
                                    RenderFrameHost* render_frame_host);

  // Gets the media player manager associated with |render_frame_host|. Creates
  // a new one if it doesn't exist. The caller doesn't own the returned pointer.
  BrowserMediaPlayerManagerTizen* GetMediaPlayerManager(
      RenderFrameHost* render_frame_host);

 private:
  // Map from RenderFrameHost* to BrowserMediaPlayerManagerTizen.
  typedef base::ScopedPtrHashMap<uintptr_t, BrowserMediaPlayerManagerTizen>
      MediaPlayerManagerMap;
  MediaPlayerManagerMap media_player_managers_;

  DISALLOW_COPY_AND_ASSIGN(MediaWebContentsObserverTizen);
};

}  // namespace content

#endif  // CONTENT_BROWSER_MEDIA_MEDIA_WEB_CONTENTS_OBSERVER_TIZEN_H_
