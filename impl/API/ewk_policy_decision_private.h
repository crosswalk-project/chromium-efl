/*
 * Copyright (C) 2014 Samsung Electronics
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef EWK_POLICY_DECISION_PRIVATE_H_
#define EWK_POLICY_DECISION_PRIVATE_H_

#include <Eina.h>

#include <base/memory/scoped_ptr.h>
#include <base/memory/ref_counted.h>
#include <base/strings/string16.h>
#include <url/gurl.h>

#include <browser/navigation_policy_handler_efl.h>
#include <browser/policy_response_delegate_efl.h>
#include <public/ewk_policy_decision.h>
#include <public/ewk_frame.h>

namespace content {
class WebContentsDelegateEfl;
class RenderViewHost;
} // namespace content

class _Ewk_Policy_Decision {
 public:
 /**
  * Constructs _Ewk_Policy_Decision with navigation type POLICY_RESPONSE
  */
  explicit _Ewk_Policy_Decision(const GURL& request_url, const net::HttpResponseHeaders* response_headers, PolicyResponseDelegateEfl* delegate);

 /**
  * Constructs _Ewk_Policy_Decision with navigation type POLICY_NAVIGATION
  */
  explicit _Ewk_Policy_Decision(const NavigationPolicyParams &params, content::RenderViewHost* rvh);

 /**
  * Constructs _Ewk_Policy_Decision with navigation type POLICY_NEWWINDOW
  */
  explicit _Ewk_Policy_Decision(content::WebContentsDelegateEfl* view, const GURL& url, const base::string16& frame);

  ~_Ewk_Policy_Decision();

  void Use();
  void Ignore();
  void Download();
  void Suspend();

  bool isDecided() const { return isDecided_; }
  bool isSuspended() const { return isSuspended_; }
  bool isMainFrame() const { return isMainFrame_; }
  Ewk_Policy_Navigation_Type GetNavigationType() const { return navigationType_; }
  Eina_Stringshare* GetCookie() const { return cookie_; }
  Eina_Stringshare* GetAuthUser() const { return AuthUser_; }
  Eina_Stringshare* GetAuthPassword() const { return AuthPassword_; }
  Eina_Stringshare* GetUrl() const { return url_; }
  Eina_Stringshare* GetScheme() const { return scheme_; }
  Eina_Stringshare* GetHost() const { return host_; }
  Eina_Stringshare* GetResponseMime() const { return responseMime_; }
  Ewk_Policy_Decision_Type GetDecisionType() const { return decisionType_; }
  Eina_Hash* GetResponseHeaders() const { return responseHeaders_; }
  int GetResponseStatusCode() const { return responseStatusCode_; }

  NavigationPolicyHandlerEfl* GetNavigationPolicyHandler() const { return navigation_policy_handler_.get(); }
  Ewk_Frame_Ref GetFrameRef() const;

 private:
  void ParseUrl(const GURL& url);

  enum PolicyType {
    POLICY_RESPONSE,
    POLICY_NAVIGATION,
    POLICY_NEWWINDOW,
  };

  content::WebContentsDelegateEfl* new_window_policy_delegate_;
  scoped_refptr<PolicyResponseDelegateEfl> policy_response_delegate_;
  scoped_ptr<NavigationPolicyHandlerEfl> navigation_policy_handler_;
  int frame_id_;
  int process_id_;
  const char* cookie_;
  const char* url_;
  const char* host_;
  const char* scheme_;
  const char* responseMime_;
  Eina_Hash* responseHeaders_;
  Ewk_Policy_Decision_Type decisionType_;
  Ewk_Policy_Navigation_Type navigationType_;
  base::string16 frame_name_;
  bool isDecided_;
  bool isSuspended_;
  int responseStatusCode_;
  const char* AuthUser_;
  const char* AuthPassword_;
  bool isMainFrame_;
  PolicyType type_;
};

#endif /* EWK_POLICY_DECISION_PRIVATE_H_ */
