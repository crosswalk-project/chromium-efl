// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TIZEN_WEBVIEW_PUBLIC_TW_COOKIE_ACCEPT_POLICY_H_
#define TIZEN_WEBVIEW_PUBLIC_TW_COOKIE_ACCEPT_POLICY_H_

namespace tizen_webview {

enum Cookie_Accept_Policy {
  TW_COOKIE_ACCEPT_POLICY_ALWAYS,        // Accepts every cookie sent from any page.
  TW_COOKIE_ACCEPT_POLICY_NEVER,         // Rejects all cookies.
  TW_COOKIE_ACCEPT_POLICY_NO_THIRD_PARTY // Accepts only cookies set by the main document loaded.
};

} // namespace tizen_webview

#endif  // TIZEN_WEBVIEW_PUBLIC_TW_COOKIE_ACCEPT_POLICY_H_
