// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef EWK_POLICY_DECISION_PRIVATE_H_
#define EWK_POLICY_DECISION_PRIVATE_H_

#include <Eina.h>

#include <string>

#include <base/memory/scoped_ptr.h>
#include <base/memory/ref_counted.h>
#include <url/gurl.h>

#include <browser/navigation_policy_handler_efl.h>
#include <browser/policy_response_delegate_efl.h>

#include "tizen_webview/public/tw_frame.h"

struct NavigationPolicyParams;

#include "content/public/common/resource_type.h"
#include "tizen_webview/public/tw_policy_decision.h"

// Basic type of authorization - 'type username:password'
#define BASIC_AUTHORIZATION "BASIC"

struct NavigationPolicyParams;

namespace net {
class URLRequest;
}

namespace content {
class WebContentsDelegateEfl;
class RenderViewHost;
} // namespace content

class _Ewk_Policy_Decision {
 public:
 /**
  * Constructs _Ewk_Policy_Decision with navigation type POLICY_RESPONSE
  */
  explicit _Ewk_Policy_Decision(const GURL& request_url,
                                net::URLRequest* request,
                                content::ResourceType resource_type,
                                PolicyResponseDelegateEfl* delegate);

 /**
  * Constructs _Ewk_Policy_Decision with navigation type POLICY_NAVIGATION
  */
  explicit _Ewk_Policy_Decision(const NavigationPolicyParams &params, content::RenderViewHost* rvh);

 /**
  * Constructs _Ewk_Policy_Decision with navigation type POLICY_NEWWINDOW
  */
  explicit _Ewk_Policy_Decision(content::WebContentsDelegateEfl* view, const GURL& url);

  ~_Ewk_Policy_Decision();

  void Use();
  void Ignore();
  void Download();
  void Suspend();

  bool isDecided() const { return isDecided_; }
  bool isSuspended() const { return isSuspended_; }
  tizen_webview::Policy_Navigation_Type GetNavigationType() const { return navigationType_; }
  const char* GetCookie() const;
  const char* GetAuthUser() const;
  const char* GetAuthPassword() const;
  const char* GetUrl() const;
  const char* GetHttpMethod() const;
  const char* GetScheme() const;
  const char* GetHost() const;
  const char* GetResponseMime() const;
  tizen_webview::Policy_Decision_Type GetDecisionType() const { return decisionType_; }
  Eina_Hash* GetResponseHeaders() const { return responseHeaders_; }
  int GetResponseStatusCode() const { return responseStatusCode_; }

  NavigationPolicyHandlerEfl* GetNavigationPolicyHandler() const { return navigation_policy_handler_.get(); }

  tizen_webview::Frame* GetFrameRef() const;

  void InitializeOnUIThread();

 private:
  void ParseUrl(const GURL& url);

  /**
   * @brief sets userID and password
   * @param request_url requested url - user:password@address
   */
  void SetAuthorizationIfNecessary(const GURL& request_url);

  /**
   * @brief sets userID and password
   * @param request contents of Authorization HTTP header
   */
  void SetAuthorizationIfNecessary(const std::string request);

  enum PolicyType {
    POLICY_RESPONSE,
    POLICY_NAVIGATION,
    POLICY_NEWWINDOW,
  };

  content::WebContentsDelegateEfl* new_window_policy_delegate_;
  scoped_refptr<PolicyResponseDelegateEfl> policy_response_delegate_;
  scoped_ptr<NavigationPolicyHandlerEfl> navigation_policy_handler_;
  scoped_ptr<tizen_webview::Frame> frame_;
  std::string cookie_;
  std::string url_;
  std::string httpMethod_;
  std::string host_;
  std::string scheme_;
  std::string responseMime_;
  Eina_Hash* responseHeaders_;
  tizen_webview::Policy_Decision_Type decisionType_;
  tizen_webview::Policy_Navigation_Type navigationType_;
  bool isDecided_;
  bool isSuspended_;
  int responseStatusCode_;
  std::string AuthUser_;
  std::string AuthPassword_;
  PolicyType type_;
};

#endif /* EWK_POLICY_DECISION_PRIVATE_H_ */
