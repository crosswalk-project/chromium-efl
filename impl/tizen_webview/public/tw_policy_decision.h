// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TIZEN_WEBVIEW_PUBLIC_TW_POLICY_DECISION_H_
#define TIZEN_WEBVIEW_PUBLIC_TW_POLICY_DECISION_H_

namespace tizen_webview {

enum Policy_Decision_Type {
  TW_POLICY_DECISION_USE,
  TW_POLICY_DECISION_DOWNLOAD,
  TW_POLICY_DECISION_IGNORE
};

enum Policy_Navigation_Type {
  TW_POLICY_NAVIGATION_TYPE_LINK_CLICKED = 0,
  TW_POLICY_NAVIGATION_TYPE_FORM_SUBMITTED = 1,
  TW_POLICY_NAVIGATION_TYPE_BACK_FORWARD = 2,
  TW_POLICY_NAVIGATION_TYPE_RELOAD = 3,
  TW_POLICY_NAVIGATION_TYPE_FORM_RESUBMITTED = 4,
  TW_POLICY_NAVIGATION_TYPE_OTHER = 5
};

} // namespace tizen_webview

#endif  // TIZEN_WEBVIEW_PUBLIC_TW_POLICY_DECISION_H_
