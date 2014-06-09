// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TIZEN_WEBVIEW_PUBLIC_TW_TOUCH_POINT_H_
#define TIZEN_WEBVIEW_PUBLIC_TW_TOUCH_POINT_H_

#include <Evas.h>

namespace tizen_webview {

struct Touch_Point {
  int id;                       // identifier of the touch event
  int x;                        // the horizontal position of the touch event
  int y;                        // the vertical position of the touch event
  Evas_Touch_Point_State state; // state of the touch event

 public:
  Touch_Point();
};

} // namespace tizen_webview

#endif  // TIZEN_WEBVIEW_PUBLIC_TW_TOUCH_POINT_H_
