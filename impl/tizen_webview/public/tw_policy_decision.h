// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TIZEN_WEBVIEW_PUBLIC_TW_POLICY_DECISION_H_
#define TIZEN_WEBVIEW_PUBLIC_TW_POLICY_DECISION_H_

#include <Eina.h>
#include <tizen_webview/public/tw_macro.h>

// TODO: break below dependency
namespace content {
class WebContentsDelegateEfl;
class RenderViewHost;

}

// TODO: break below dependency
namespace net  {
class HttpResponseHeaders;
}

// TODO: break below dependency
class NavigationPolicyHandlerEfl;
class Ewk_Frame;
class GURL;
class _Ewk_Policy_Decision;
class PolicyResponseDelegateEfl;
struct NavigationPolicyParams;

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

// TODO: refactoring required for break dependency to chromium internals
class PolicyDecision {
 public:
  // for type POLICY_RESPONSE
  PolicyDecision(const GURL& request_url, const net::HttpResponseHeaders* response_headers, PolicyResponseDelegateEfl* delegate);
  // for POLICY_NAVIGATION
  PolicyDecision(const NavigationPolicyParams &params, content::RenderViewHost* rvh);
  // for POLICY_NEWWINDOW
  PolicyDecision(content::WebContentsDelegateEfl* view, const GURL& url);

  ~PolicyDecision();

  void Use();
  void Ignore();
  void Download();
  void Suspend();

  bool isDecided() const;
  bool isSuspended() const;

  const char* GetCookie() const;
  const char* GetAuthUser() const;
  const char* GetAuthPassword() const;
  const char* GetUrl() const;
  const char* GetScheme() const;
  const char* GetHost() const;
  const char* GetResponseMime() const;
  tizen_webview::Policy_Navigation_Type GetNavigationType() const;
  tizen_webview::Policy_Decision_Type GetDecisionType() const;
  Eina_Hash* GetResponseHeaders() const;
  int GetResponseStatusCode() const;
  NavigationPolicyHandlerEfl* GetNavigationPolicyHandler() const;
  Ewk_Frame* GetFrameRef() const;
  //void InitializeOnUIThread();
 private:
  typedef ::_Ewk_Policy_Decision Impl;
  Impl* impl_;
  Impl* GetImpl() { return impl_; }

  DISALLOW_COPY_AND_ASSIGN(PolicyDecision);
  friend class ::PolicyResponseDelegateEfl;
}; // class PolicyDecision

} // namespace tizen_webview

#endif  // TIZEN_WEBVIEW_PUBLIC_TW_POLICY_DECISION_H_
