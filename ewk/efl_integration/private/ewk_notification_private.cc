// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ewk_notification_private.h"


// Use compiler generated constructor and destructor
// No significant implementation is required.

Ewk_Notification* chromium_glue::from(tizen_webview::Notification* val) {
  // Down-casting is safe because it is the only class that inherits
  // tizen_webview::Notification
  return static_cast<Ewk_Notification*>(val);
}
