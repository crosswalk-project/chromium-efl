// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TIZEN_WEBVIEW_PUBLIC_TW_TOUCH_EVENT_H_
#define TIZEN_WEBVIEW_PUBLIC_TW_TOUCH_EVENT_H_

namespace tizen_webview {

enum Touch_Event_Type {
  TW_TOUCH_START,
  TW_TOUCH_MOVE,
  TW_TOUCH_END,
  TW_TOUCH_CANCEL
};

} // namespace tizen_webview

#endif  // TIZEN_WEBVIEW_PUBLIC_TW_TOUCH_EVENT_H_
