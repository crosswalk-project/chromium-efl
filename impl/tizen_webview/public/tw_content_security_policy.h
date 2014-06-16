// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TIZEN_WEBVIEW_PUBLIC_TW_CONTENT_SECURITY_POLICY_H_
#define TIZEN_WEBVIEW_PUBLIC_TW_CONTENT_SECURITY_POLICY_H_

namespace tizen_webview {

enum ContentSecurityPolicyType {
  TW_CSP_REPORT_ONLY,
  TW_CSP_ENFORCE_POLICY,
  TW_CSP_DEFAULT_POLICY
};
} // namespace tizen_webview

#endif  // TIZEN_WEBVIEW_PUBLIC_TW_CONTENT_SECURITY_POLICY_H_
