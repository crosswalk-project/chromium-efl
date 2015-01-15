// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TIZEN_WEBVIEW_PUBLIC_TW_NOTIFICATION_H_
#define TIZEN_WEBVIEW_PUBLIC_TW_NOTIFICATION_H_

#include <Evas.h>
#include <string>

#include "base/callback.h"
#include "base/memory/scoped_ptr.h"

#include "tizen_webview/public/tw_macro.h"
#include "tizen_webview/public/tw_security_origin.h"

struct _Ewk_Notification;
struct _Ewk_Notification_Permission_Request;

namespace content {
class NotificationControllerEfl;
class WebContents;
}

namespace tizen_webview {

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
  std::string body_;
  std::string iconURL_;
  std::string replaceID_;
  std::string title_;
  uint64_t notificationID_;
  scoped_ptr<Security_Origin> origin_;

  DISALLOW_COPY_AND_ASSIGN(Notification);
};

class NotificationPermissionRequest {
 public:
  NotificationPermissionRequest(Evas_Object* webview,
                                const base::Callback<void(bool)>& callback,
                                const tizen_webview::URL& source_origin);
  ~NotificationPermissionRequest();

  Evas_Object* GetWebviewEvasObject() const;
  const Security_Origin* GetSecurityOrigin() const;
  bool IsDecided() const;
  bool IsSuspended() const;
  void SetSuspend(bool suspend) const;
  void Reply(bool allow);

 private:
  Evas_Object* webview_;
  scoped_ptr<Security_Origin> origin_;
  base::Callback<void(bool)> callback_;
  bool decided_;
  mutable bool suspended_;

  DISALLOW_COPY_AND_ASSIGN(NotificationPermissionRequest);
};

} // namespace tizen_webview

#endif  // TIZEN_WEBVIEW_PUBLIC_TW_NOTIFICATION_H_
