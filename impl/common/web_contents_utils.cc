// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "common/web_contents_utils.h"

#include "content/public/browser/render_view_host.h"
#include "content/public/browser/render_frame_host.h"
#include "content/public/browser/web_contents.h"

#include "browser_context_efl.h"

using content::WebContents;
using content::RenderFrameHost;
using content::RenderViewHost;

namespace web_contents_utils {

WebContents* WebContentsFromViewID(int render_process_id, int render_view_id)
{
    RenderViewHost* render_view_host = RenderViewHost::FromID(render_process_id, render_view_id);

    if (!render_view_host)
        return NULL;

    return WebContents::FromRenderViewHost(render_view_host);
}

WebContents* WebContentsFromFrameID(int render_process_id, int render_frame_id)
{
    RenderFrameHost* render_frame_host = RenderFrameHost::FromID(render_process_id, render_frame_id);

    if (!render_frame_host)
        return NULL;

    RenderViewHost* render_view_host = render_frame_host->GetRenderViewHost();

    if (!render_view_host)
        return NULL;

    return WebContents::FromRenderViewHost(render_view_host);
}

} // namespace web_contents_utils
