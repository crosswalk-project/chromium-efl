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

namespace content {
class WebContentsDelegateEfl;
} // namespace content

struct _Ewk_Policy_Decision {

  explicit _Ewk_Policy_Decision(PolicyResponseDelegateEfl* delegate)
      : new_window_policy_delegate(NULL),
        policy_response_delegate(delegate),
        cookie(0),
        url(0),
        host(0),
        scheme(0),
        responseMime(0),
        responseHeaders(0),
        decisionType(EWK_POLICY_DECISION_USE),
        navigationType(EWK_POLICY_NAVIGATION_TYPE_OTHER),
        isDecided(false),
        isSuspended(false),
        responseStatusCode(0),
        AuthUser(0),
        AuthPassword(0),
        isMainFrame(true),
        type(POLICY_RESPONSE) {
  }

  _Ewk_Policy_Decision(const GURL& request_url, Ewk_Policy_Navigation_Type nav_type, bool is_main_frame, NavigationPolicyHandlerEfl* delegate)
      : new_window_policy_delegate(NULL),
        navigation_policy_handler(delegate),
        requestUrl(request_url),
        cookie(0),
        url(0),
        host(0),
        scheme(0),
        responseMime(0),
        responseHeaders(0),
        decisionType(EWK_POLICY_DECISION_USE),
        navigationType(nav_type),
        isDecided(false),
        isSuspended(false),
        responseStatusCode(0),
        AuthUser(0),
        AuthPassword(0),
        isMainFrame(is_main_frame),
        type(POLICY_NAVIGATION) {
  }

  _Ewk_Policy_Decision(content::WebContentsDelegateEfl* view, const GURL& url, const base::string16& frame)
      : new_window_policy_delegate(view),
        requestUrl(url),
        cookie(0),
        url(0),
        host(0),
        scheme(0),
        responseMime(0),
        responseHeaders(0),
        decisionType(EWK_POLICY_DECISION_USE),
        navigationType(EWK_POLICY_NAVIGATION_TYPE_OTHER),
        frame_name(frame),
        isDecided(false),
        isSuspended(false),
        responseStatusCode(0),
        AuthUser(0),
        AuthPassword(0),
        isMainFrame(true),
        type(POLICY_NEWWINDOW) {
  }

  ~_Ewk_Policy_Decision() {
    eina_stringshare_del(cookie);
    eina_stringshare_del(url);
    eina_stringshare_del(host);
    eina_stringshare_del(scheme);
    eina_stringshare_del(responseMime);
    eina_hash_free(responseHeaders);
    eina_stringshare_del(AuthUser);
    eina_stringshare_del(AuthPassword);
  }
  enum PolicyType {
    POLICY_RESPONSE,
    POLICY_NAVIGATION,
    POLICY_NEWWINDOW,
  };

  void Use();

  content::WebContentsDelegateEfl* new_window_policy_delegate;
  scoped_refptr<PolicyResponseDelegateEfl> policy_response_delegate;
  scoped_ptr<NavigationPolicyHandlerEfl> navigation_policy_handler;
  GURL requestUrl;
  const char* cookie;
  const char* url;
  const char* host;
  const char* scheme;
  const char* responseMime;
  Eina_Hash* responseHeaders;
  Ewk_Policy_Decision_Type decisionType;
  Ewk_Policy_Navigation_Type navigationType;
  base::string16 frame_name;
  bool isDecided;
  bool isSuspended;
  int responseStatusCode;
  const char* AuthUser;
  const char* AuthPassword;
  bool isMainFrame;
  PolicyType type;
};

#endif /* EWK_POLICY_DECISION_PRIVATE_H_ */
