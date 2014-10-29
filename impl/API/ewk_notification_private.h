// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TIZEN_WEBVIEW_PRIVATE_TW_NOTIFICATION_IMPL_H_
#define TIZEN_WEBVIEW_PRIVATE_TW_NOTIFICATION_IMPL_H_

#include <Eina.h>
#include <Evas.h>

#include "base/strings/utf_string_conversions.h"
#include "content/public/common/show_desktop_notification_params.h"

#include "eweb_view.h"
#include "eweb_context.h"

namespace tizen_webview {
class Security_Origin;
class URL;
}

struct _Ewk_Notification {
  std::string body_;
  std::string iconURL_;
  std::string replaceID_;
  std::string title_;
  uint64_t notificationID_;
  tizen_webview::Security_Origin* securityOrigin_;

  _Ewk_Notification(const std::string& body,
                    const std::string& iconURL,
                    const std::string& replaceID,
                    const std::string& title,
                    uint64_t notificationID,
                    const tizen_webview::URL& securityOrigin);
  ~_Ewk_Notification();
};

struct _Ewk_Notification_Permission_Request {
  Evas_Object* webview_;
  tizen_webview::Security_Origin* origin_;
  bool decided_;
  bool suspended_;
  int callback_context_; // required by content API

  _Ewk_Notification_Permission_Request(Evas_Object* webview,
      int callback_context, const tizen_webview::URL& source_origin);
  ~_Ewk_Notification_Permission_Request();
};

#endif // TIZEN_WEBVIEW_PRIVATE_TW_NOTIFICATION_IMPL_H_
