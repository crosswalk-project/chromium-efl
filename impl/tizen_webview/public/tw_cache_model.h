// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TIZEN_WEBVIEW_PUBLIC_TW_CACHE_MODEL_H_
#define TIZEN_WEBVIEW_PUBLIC_TW_CACHE_MODEL_H_

namespace tizen_webview {

enum Cache_Model {
  TW_CACHE_MODEL_DOCUMENT_VIEWER,   // Use the smallest cache capacity.
  TW_CACHE_MODEL_DOCUMENT_BROWSER,  // Use bigger cache capacity than TW_CACHE_MODEL_DOCUMENT_VIEWER.
  TW_CACHE_MODEL_PRIMARY_WEBBROWSER // Use the biggest cache capacity.
};

} // namespace tizen_webview

#endif  // TIZEN_WEBVIEW_PUBLIC_TW_CACHE_MODEL_H_
