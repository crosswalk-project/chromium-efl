// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TIZEN_WEBVIEW_PUBLIC_TW_SETTINGS_H_
#define TIZEN_WEBVIEW_PUBLIC_TW_SETTINGS_H_

namespace tizen_webview {
struct Settings {
  Settings() : javascript_can_open_windows(false) {}
  bool javascript_can_open_windows;
};

} // namespace tizen_webview

#endif // TIZEN_WEBVIEW_PUBLIC_TW_SETTINGS_H_
