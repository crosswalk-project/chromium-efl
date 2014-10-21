// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "tw_frame.h"

#include <content/public/browser/render_frame_host.h>
#include <content/public/browser/render_process_host.h>
#include <content/public/browser/render_view_host.h>
#include <content/public/browser/web_contents.h>

#include <navigation_policy_params.h>

using content::RenderFrameHost;
using content::RenderViewHost;
using content::WebContents;

namespace tizen_webview {
Frame::Frame(RenderFrameHost* rfh)
  : is_main_frame_(false) {
  if (!rfh)
    return;
  RenderViewHost *rvh = rfh->GetRenderViewHost();
  if (!rvh)
    return;
  WebContents *wc = WebContents::FromRenderViewHost(rvh);
  if (!wc)
    return;
  is_main_frame_ = wc->GetMainFrame() == rfh;
}

Frame::Frame(const NavigationPolicyParams &params)
  : is_main_frame_(params.is_main_frame)
{
}
} //namespace tizen_webview
