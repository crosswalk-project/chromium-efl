// Copyright 2013 The Chromium Authors. All rights reserved.
// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "network_delegate_efl.h"

#include "content/public/browser/browser_thread.h"
#include "net/base/net_errors.h"
#include "net/http/http_response_headers.h"

namespace net {

NetworkDelegateEfl::NetworkDelegateEfl(CookieManager& cookie_manager)
  : cookie_manager_(cookie_manager) {
}

NetworkDelegate::AuthRequiredResponse NetworkDelegateEfl::OnAuthRequired(
    URLRequest* request,
    const AuthChallengeInfo& auth_info,
    const AuthCallback& callback,
    AuthCredentials* credentials) {
  return AUTH_REQUIRED_RESPONSE_NO_ACTION;
}

bool NetworkDelegateEfl::OnCanGetCookies(const URLRequest& request,
                                         const CookieList& cookie_list) {
  return cookie_manager_.OnCanGetCookies(request, cookie_list);
}

bool NetworkDelegateEfl::OnCanSetCookie(const URLRequest& request,
                                        const std::string& cookie_line,
                                        CookieOptions* options) {
  return cookie_manager_.OnCanSetCookie(request, cookie_line, options);
}

bool NetworkDelegateEfl::OnCanAccessFile(const URLRequest& request,
                                         const base::FilePath& path) const {
  return true;
}

bool NetworkDelegateEfl::OnCanThrottleRequest(const URLRequest& request) const {
  return true;
}


}; // namespace net
