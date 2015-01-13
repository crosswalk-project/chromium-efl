// Copyright 2013 The Chromium Authors. All rights reserved.
// Copyright 2014 Samsung Electronics. All rights reseoved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _NETWORK_DELEGATE_EFL_H_
#define _NETWORK_DELEGATE_EFL_H_

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "net/base/network_delegate.h"
#include "cookie_manager.h"

namespace net {

class NetworkDelegateEfl : public NetworkDelegate {
 public:
  NetworkDelegateEfl(base::WeakPtr<CookieManager> cookie_manager);

 private:
  // NetworkDelegate implementation.
  virtual AuthRequiredResponse OnAuthRequired(
      URLRequest* request,
      const AuthChallengeInfo& auth_info,
      const AuthCallback& callback,
      AuthCredentials* credentials) override;
  virtual bool OnCanGetCookies(const URLRequest& request,
                               const CookieList& cookie_list) override;
  virtual bool OnCanSetCookie(const URLRequest& request,
                              const std::string& cookie_line,
                              CookieOptions* options) override;
  virtual bool OnCanAccessFile(const URLRequest& request,
                               const base::FilePath& path) const override;
  virtual bool OnCanThrottleRequest(const URLRequest& request) const override;

  base::WeakPtr<CookieManager> cookie_manager_;
};

} // namespace net

#endif // _NETWORK_DELEGATE_EFL_H_
