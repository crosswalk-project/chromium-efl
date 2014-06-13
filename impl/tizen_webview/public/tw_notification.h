// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TIZEN_WEBVIEW_PUBLIC_TW_NOTIFICATION_H_
#define TIZEN_WEBVIEW_PUBLIC_TW_NOTIFICATION_H_

#include <Evas.h>
#include <string>
#include "tizen_webview/public/tw_macro.h"

struct _Ewk_Notification;
struct _Ewk_Notification_Permission_Request;

namespace content {
class NotificationControllerEfl;
}

namespace tizen_webview {

class Security_Origin;
class URL;

class Notification {
 public:
  Notification(const std::string& body,
               const std::string& iconURL,
               const std::string& replaceID,
               const std::string& title,
               uint64_t notificationID,
               const tizen_webview::URL& securityOrigin);
  ~Notification();

  const char* GetBody() const;
  const char* GetIconUrl() const;
  const char* GetReplaceId() const;
  const char* GetTitle() const;
  int GetID() const;
  const Security_Origin* GetSecurityOrigin() const;

 private:
  typedef ::_Ewk_Notification Impl;
  Impl *impl;
  DISALLOW_COPY_AND_ASSIGN(Notification);
};

class NotificationPermissionRequest {
 public:
  NotificationPermissionRequest(Evas_Object* webview,
                                int callback_context,
                                const tizen_webview::URL& source_origin);
  ~NotificationPermissionRequest();

  Evas_Object* GetWebviewEvasObject() const;
  const Security_Origin* GetSecurityOrigin() const;
  bool IsDecided() const;
  bool IsSuspended() const;
  void SetSuspend(bool suspend) const;

 private:
  // get chromium internal callback context
  int GetInternalCallbackContext() const;
  friend class content::NotificationControllerEfl;

  typedef ::_Ewk_Notification_Permission_Request Impl;
  Impl *impl;


  DISALLOW_COPY_AND_ASSIGN(NotificationPermissionRequest);
};

} // namespace tizen_webview

#endif  // TIZEN_WEBVIEW_PUBLIC_TW_NOTIFICATION_H_
