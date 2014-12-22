// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TIZEN_WEBVIEW_PUBLIC_TW_ERROR_H_
#define TIZEN_WEBVIEW_PUBLIC_TW_ERROR_H_

#include <string>
#include "url/gurl.h"

namespace tizen_webview {
struct Error {
  GURL url;
  bool is_main_frame;
  int code;
  std::string description;
  std::string domain;
};
} // namespace tizen_webview

#endif // TIZEN_WEBVIEW_PUBLIC_TW_ERROR_H_
