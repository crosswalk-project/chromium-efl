// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TIZEN_WEBVIEW_PUBLIC_TW_MISC_UTILITY_H_
#define TIZEN_WEBVIEW_PUBLIC_TW_MISC_UTILITY_H_

#include "url/gurl.h"
#include "tizen_webview/public/tw_url.h"

namespace tizen_webview {

GURL GetGURL(const tizen_webview::URL& url);
tizen_webview::URL  GetURL(const GURL& url);

} // namespace tizen_webview

#endif  // TIZEN_WEBVIEW_PUBLIC_TW_MISC_UTILITY_H_
