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
#include "browser/policy_response_delegate_efl.h"

namespace net {

class NetworkDelegateEfl : public NetworkDelegate {
 public:
  NetworkDelegateEfl(CookieManager& cookie_manager);
  virtual ~NetworkDelegateEfl();

 private:
  // NetworkDelegate implementation.
  virtual int OnBeforeURLRequest(URLRequest* request,
                                 const CompletionCallback& callback,
                                 GURL* new_url) override;
  virtual int OnBeforeSendHeaders(URLRequest* request,
                                  const CompletionCallback& callback,
                                  HttpRequestHeaders* headers) override;
  virtual void OnSendHeaders(URLRequest* request,
                             const HttpRequestHeaders& headers) override;
  virtual int OnHeadersReceived(
      URLRequest* request,
      const CompletionCallback& callback,
      const HttpResponseHeaders* original_response_headers,
      scoped_refptr<HttpResponseHeaders>* override_response_headers,
      GURL* allowed_unsafe_redirect_url) override;
  virtual void OnBeforeRedirect(URLRequest* request,
                                const GURL& new_location) override;
  virtual void OnResponseStarted(URLRequest* request) override;
  virtual void OnRawBytesRead(const URLRequest& request, int bytes_read) override;
  virtual void OnCompleted(URLRequest* request, bool started) override;
  virtual void OnURLRequestDestroyed(URLRequest* request) override;
  virtual void OnPACScriptError(int line_number,
                                const base::string16& error) override;
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
  virtual int OnBeforeSocketStreamConnect(
      SocketStream* stream,
      const CompletionCallback& callback) override;

  typedef base::hash_map<uint64, scoped_refptr<PolicyResponseDelegateEfl> > ResponsePolicyDelegateTable;
  ResponsePolicyDelegateTable policy_response_delegate_map_;

  CookieManager& cookie_manager_;
};

} // namespace net

#endif // _NETWORK_DELEGATE_EFL_H_
