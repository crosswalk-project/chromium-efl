// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TIZEN_WEBVIEW_PUBLIC_TW_NOTIFICATION_H_
#define TIZEN_WEBVIEW_PUBLIC_TW_NOTIFICATION_H_

#include <Evas.h>
#include <string>
#include "tizen_webview/public/tw_macro.h"

struct _Ewk_Notification;

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

} // namespace tizen_webview

#endif  // TIZEN_WEBVIEW_PUBLIC_TW_NOTIFICATION_H_
