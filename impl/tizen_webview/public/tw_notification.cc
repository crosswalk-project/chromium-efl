// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "tw_notification.h"

#include "API/ewk_notification_private.h"

namespace tizen_webview {

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

} // namespace tizen_webview
