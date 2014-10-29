// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ewk_notification_private_h
#define ewk_notification_private_h

#include <tizen_webview/public/tw_notification.h>

struct Ewk_Notification : public tizen_webview::Notification {
};

struct Ewk_Notification_Permission_Request
    : public tizen_webview::NotificationPermissionRequest {
};

namespace chromium_glue {
  Ewk_Notification* from(tizen_webview::Notification*);

  //tizen_webview::NotificationPermissionRequest* to(tizen_webview::Notification*);
}

#endif // ewk_notification_private_h
