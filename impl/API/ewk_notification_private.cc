// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ewk_notification_private.h"
#include "tizen_webview/public/tw_url.h"
#include "tizen_webview/public/tw_security_origin.h"

// _Ewk_Notification ----------------------------------------------------------
_Ewk_Notification::_Ewk_Notification(
    const std::string& body,
    const std::string& iconURL,
    const std::string& replaceID,
    const std::string& title,
    uint64_t notificationID,
    const tizen_webview::URL& origin)
    : body_(body),
      iconURL_(iconURL),
      replaceID_(replaceID),
      title_(title),
#if 0
      notificationID_(notificationID),
#else
#warning "[M37] params.notification_id does not exist"
      notificationID_(0),
#endif
      securityOrigin_(new tizen_webview::Security_Origin(origin.getHost(),
                                                         origin.getProtocol(),
                                                         origin.getPort())) {
}

_Ewk_Notification::~_Ewk_Notification() {
  delete securityOrigin_;
}

// _Ewk_Notification_Permission_Request ---------------------------------------
_Ewk_Notification_Permission_Request::_Ewk_Notification_Permission_Request(
    Evas_Object* webview, int callback_context,
    const tizen_webview::URL& source_origin)
  : webview_(webview),
    origin_(new tizen_webview::Security_Origin(source_origin)),
    decided_(false),
    suspended_(false),
    callback_context_(callback_context) {
}

_Ewk_Notification_Permission_Request::~_Ewk_Notification_Permission_Request() {
  delete origin_;
}
