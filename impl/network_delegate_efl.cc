// Copyright 2013 The Chromium Authors. All rights reserved.
// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "network_delegate_efl.h"

#include "content/public/browser/browser_thread.h"
#include "net/base/net_errors.h"
#include "net/http/http_response_headers.h"

#include "API/ewk_policy_decision_private.h"

namespace net {

namespace {

static bool IsRedirect(int status_code) {
  return 300 <= status_code && status_code < 400 && status_code != 304;
}

static bool ShouldInvokeResponseCallback(
    const HttpResponseHeaders* response_headers) {
  return (response_headers &&
      !IsRedirect(response_headers->response_code()) &&
      !response_headers->HasHeader("Location"));
}

};

NetworkDelegateEfl::NetworkDelegateEfl(CookieManager& cookie_manager)
  : cookie_manager_(cookie_manager) {
}

NetworkDelegateEfl::~NetworkDelegateEfl() {
}

int NetworkDelegateEfl::OnBeforeURLRequest(
    URLRequest* request,
    const CompletionCallback& callback,
    GURL* new_url) {
  return OK;
}

int NetworkDelegateEfl::OnBeforeSendHeaders(
    URLRequest* request,
    const CompletionCallback& callback,
    HttpRequestHeaders* headers) {
  return OK;
}

void NetworkDelegateEfl::OnSendHeaders(
    URLRequest* request,
    const HttpRequestHeaders& headers) {
}

int NetworkDelegateEfl::OnHeadersReceived(URLRequest* request,
    const CompletionCallback& callback,
    const HttpResponseHeaders* original_response_headers,
    scoped_refptr<HttpResponseHeaders>* override_response_headers,
    GURL* allowed_unsafe_redirect_url) {
  DCHECK(content::BrowserThread::CurrentlyOn(content::BrowserThread::IO));
  if(!ShouldInvokeResponseCallback(original_response_headers))
    return OK;

  scoped_refptr<PolicyResponseDelegateEfl> policy_delegate =
      new PolicyResponseDelegateEfl(request, callback, original_response_headers);
  policy_response_delegate_map_.insert(
      std::pair< uint64, scoped_refptr<PolicyResponseDelegateEfl> >(
        request->identifier(), policy_delegate));
  return ERR_IO_PENDING;
}

void NetworkDelegateEfl::OnBeforeRedirect(URLRequest* request,
                                          const GURL& new_location) {
}

void NetworkDelegateEfl::OnResponseStarted(URLRequest* request) {
}

void NetworkDelegateEfl::OnRawBytesRead(const URLRequest& request,
                                        int bytes_read) {
}

void NetworkDelegateEfl::OnCompleted(URLRequest* request, bool started) {
  DCHECK(content::BrowserThread::CurrentlyOn(content::BrowserThread::IO));

  ResponsePolicyDelegateTable::const_iterator found =
      policy_response_delegate_map_.find(request->identifier());

  if (found != policy_response_delegate_map_.end() &&
      (request->status().status() == URLRequestStatus::CANCELED ||
       request->status().status() == URLRequestStatus::FAILED)) {

    found->second->HandleURLRequestDestroyedOnIOThread();
    policy_response_delegate_map_.erase(request->identifier());
  }
}

void NetworkDelegateEfl::OnURLRequestDestroyed(URLRequest* request) {
  DCHECK(content::BrowserThread::CurrentlyOn(content::BrowserThread::IO));
  DCHECK(request);
  uint64 request_id = request->identifier();
  ResponsePolicyDelegateTable::const_iterator pending_policy_delegate =
      policy_response_delegate_map_.find(request_id);

  if (pending_policy_delegate != policy_response_delegate_map_.end()) {
    pending_policy_delegate->second->HandleURLRequestDestroyedOnIOThread();
    policy_response_delegate_map_.erase(request_id);
  }
}

void NetworkDelegateEfl::OnPACScriptError(int line_number,
                                          const base::string16& error) {
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

int NetworkDelegateEfl::OnBeforeSocketStreamConnect(
      SocketStream* stream,
      const CompletionCallback& callback) {
  return OK;
}

}; // namespace net
