// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TIZEN_WEBVIEW_PUBLIC_TW_FRAME_H_
#define TIZEN_WEBVIEW_PUBLIC_TW_FRAME_H_

struct NavigationPolicyParams;

namespace content {
class RenderFrameHost;
}

namespace tizen_webview {
class Frame {
public:
  Frame(content::RenderFrameHost* rfh);
  Frame(const NavigationPolicyParams &params);

  bool IsMainFrame() const { return is_main_frame_; }

private:
  bool is_main_frame_;
};
}

#endif // TIZEN_WEBVIEW_PUBLIC_TW_FRAME_H_
