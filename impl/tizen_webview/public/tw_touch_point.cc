// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "tw_touch_point.h"

namespace tizen_webview {

Touch_Point::Touch_Point()
  : id (-1), x (-1), y (-1), state (EVAS_TOUCH_POINT_CANCEL)
{
}

} // namespace tizen_webview
