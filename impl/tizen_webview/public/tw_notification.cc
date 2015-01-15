// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "tw_notification.h"

#include "content/public/browser/browser_thread.h"
#include "content/public/browser/web_contents.h"
#include "eweb_view.h"
#include "tw_web_context.h"

using content::BrowserThread;

namespace tizen_webview {

// tizen_webview::Notification ------------------------------------------------
Notification::Notification(
    const std::string& body,
    const std::string& iconURL,
    const std::string& replaceID,
    const std::string& title,
    uint64_t notificationID,
    const tizen_webview::URL& origin)
    : body_(body)
    , iconURL_(iconURL)
    , replaceID_(replaceID)
    , title_(title)
    , notificationID_(notificationID)
    , origin_(new Security_Origin(origin)) {
}

Notification::~Notification() {
}

const char* Notification::GetBody() const {
  return body_.c_str();
}

const char* Notification::GetIconUrl() const {
  return iconURL_.c_str();
}

const char* Notification::GetReplaceId() const {
  return replaceID_.c_str();
}

const char* Notification::GetTitle() const {
  return title_.c_str();
}

int Notification::GetID() const {
  return notificationID_;
}

const Security_Origin* Notification::GetSecurityOrigin() const {
  return origin_.get();
}

// tizen_webview::NotificationPermissionRequest -------------------------------

NotificationPermissionRequest::NotificationPermissionRequest(
    Evas_Object* webview,
    const base::Callback<void(bool)>& callback,
    const URL& source_origin)
    : webview_(webview)
    , origin_(new Security_Origin(source_origin))
    , callback_(callback)
    , decided_(false)
    , suspended_(false)
{
}

NotificationPermissionRequest::~NotificationPermissionRequest() {
}

Evas_Object* NotificationPermissionRequest::GetWebviewEvasObject() const {
  return webview_;
}

const Security_Origin* NotificationPermissionRequest::GetSecurityOrigin() const {
  return origin_.get();
}

bool NotificationPermissionRequest::IsDecided() const {
  return decided_;
}

bool NotificationPermissionRequest::IsSuspended() const {
  return suspended_;
}

void NotificationPermissionRequest::SetSuspend(bool suspend) const {
  suspended_ = suspend;
}

void NotificationPermissionRequest::Reply(bool allow) {
#if defined(ENABLE_NOTIFICATIONS)
  if (decided_)
    return;
  decided_ = true;
  EWebView* view = EWebView::FromEvasObject(webview_);
  if (view) {
    view->context()->browser_context()->GetNotificationController()->
        SetPermissionForNotification(this, allow);
  }
  callback_.Run(allow);

  if (suspended_) {
    // If decision was suspended, then it was not deleted by the creator
    // Deletion of this object must be done after decision was made, as
    // this object will no longer be valid. But if decision was not suspended
    // it will be deleted right after permission callbacks are executed.
    BrowserThread::DeleteSoon(BrowserThread::UI, FROM_HERE, this);
  }
#endif
}

} // namespace tizen_webview
