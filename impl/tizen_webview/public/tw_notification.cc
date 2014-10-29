// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "tw_notification.h"

#include "API/ewk_notification_private.h"

namespace tizen_webview {

// tizen_webview::Notification ------------------------------------------------
Notification::Notification(
    const std::string& body,
    const std::string& iconURL,
    const std::string& replaceID,
    const std::string& title,
    uint64_t notificationID,
    const tizen_webview::URL& origin)
    : impl(new Impl(body, iconURL, replaceID, title, notificationID, origin)) {
}

Notification::~Notification() {
  delete impl;
}

const char* Notification::GetBody() const {
  return impl->body_.c_str();
}

const char* Notification::GetIconUrl() const {
  return impl->iconURL_.c_str();
}

const char* Notification::GetReplaceId() const {
  return impl->replaceID_.c_str();
}

const char* Notification::GetTitle() const {
  return impl->title_.c_str();
}

int Notification::GetID() const {
  return impl->notificationID_;
}

const Security_Origin* Notification::GetSecurityOrigin() const {
  return impl->securityOrigin_;
}

// tizen_webview::NotificationPermissionRequest -------------------------------

NotificationPermissionRequest::NotificationPermissionRequest(
    Evas_Object* webview, int callback_context, const URL& source_origin)
    : impl(new _Ewk_Notification_Permission_Request(
          webview, callback_context,source_origin)) {
}

NotificationPermissionRequest::~NotificationPermissionRequest() {
  delete impl;
}

Evas_Object* NotificationPermissionRequest::GetWebviewEvasObject() const {
  return impl->webview_;
}

const Security_Origin* NotificationPermissionRequest::GetSecurityOrigin() const {
  return impl->origin_;
}

bool NotificationPermissionRequest::IsDecided() const {
  return impl->decided_;
}

bool NotificationPermissionRequest::IsSuspended() const {
  return impl->suspended_;
}

void NotificationPermissionRequest::SetSuspend(bool suspend) const {
  impl->suspended_ = suspend;
}

int NotificationPermissionRequest::GetInternalCallbackContext() const {
  return impl->callback_context_;
}

} // namespace tizen_webview
