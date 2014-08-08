// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "tw_webview_delegate.h"
#include <base/logging.h>

namespace tizen_webview {

static WebViewDelegate* gWebViewDelegate = NULL;

void WebViewDelegate::RegisterDelegate(WebViewDelegate* wv_delegate) {
  if (!gWebViewDelegate) {
    gWebViewDelegate = wv_delegate;
  }
}

bool WebViewDelegate::IsDelegateRegistered() {
  return gWebViewDelegate != NULL;
}

WebViewDelegate* WebViewDelegate::GetInstance() {
  if (!IsDelegateRegistered()) {
    DLOG(ERROR) << "tizen_webview::WebViewDelegate is not yet registered.";
    return NULL;
  }
  return gWebViewDelegate;
}

} // namespace tizen_webview
