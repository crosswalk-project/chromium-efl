// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "tw_policy_decision.h"

#include "API/ewk_policy_decision_private.h"

namespace tizen_webview {


PolicyDecision::PolicyDecision(
    const GURL& request_url,
    const net::HttpResponseHeaders* response_headers,
    PolicyResponseDelegateEfl* delegate)
    : impl_(new Impl(request_url, response_headers, delegate)) {
}

PolicyDecision::PolicyDecision(
    const NavigationPolicyParams &params,
    content::RenderViewHost* rvh)
    : impl_(new Impl(params, rvh)) {
}

PolicyDecision::PolicyDecision(
    content::WebContentsDelegateEfl* view,
    const GURL& url)
    : impl_(new Impl(view, url)) {
}

PolicyDecision::~PolicyDecision() {
  delete impl_;
}

void PolicyDecision::Use() {
  impl_->Use();
}

void PolicyDecision::Ignore() {
  impl_->Ignore();
}

void PolicyDecision::Download() {
  impl_->Download();
}

void PolicyDecision::Suspend() {
  impl_->Suspend();
}

bool PolicyDecision::isDecided() const {
  return impl_->isDecided();
}

bool PolicyDecision::isSuspended() const {
  return impl_->isSuspended();
}

const char* PolicyDecision::GetCookie() const {
  return impl_->GetCookie();
}

const char* PolicyDecision::GetAuthUser() const {
  return impl_->GetAuthUser();
}

const char* PolicyDecision::GetAuthPassword() const {
  return impl_->GetAuthPassword();
}

const char* PolicyDecision::GetUrl() const {
  return impl_->GetUrl();
}

const char* PolicyDecision::GetScheme() const {
  return impl_->GetScheme();
}

const char* PolicyDecision::GetHost() const {
  return impl_->GetHost();
}

const char* PolicyDecision::GetResponseMime() const {
  return impl_->GetResponseMime();
}

tizen_webview::Policy_Navigation_Type PolicyDecision::GetNavigationType() const {
  return impl_->GetNavigationType();
}

tizen_webview::Policy_Decision_Type PolicyDecision::GetDecisionType() const {
  return impl_->GetDecisionType();
}

Eina_Hash* PolicyDecision::GetResponseHeaders() const {
  return impl_->GetResponseHeaders();
}

int PolicyDecision::GetResponseStatusCode() const {
  return impl_->GetResponseStatusCode();
}

tizen_webview::Frame* PolicyDecision::GetFrameRef() const {
  return impl_->GetFrameRef();
}

} // namespace tizen_webview
