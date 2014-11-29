/*
   Copyright (C) 2013 Samsung Electronics

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "ewk_policy_decision.h"

#if !defined(EWK_BRINGUP)
#include <tizen_webview/public/tw_frame.h>
#endif
// TODO: remove dependency
#include <../impl/web_contents_delegate_efl.h>

#include "private/chromium_glue.h"
#include "private/ewk_policy_decision_private.h"
#include "private/ewk_private.h"

const char* ewk_policy_decision_cookie_get(const Ewk_Policy_Decision* policyDecision)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(policyDecision, NULL);
  return policyDecision->GetCookie();
}

const char* ewk_policy_decision_userid_get(const Ewk_Policy_Decision* policyDecision)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(policyDecision, NULL);
  return policyDecision->GetAuthUser();
}

const char* ewk_policy_decision_password_get(const Ewk_Policy_Decision* policyDecision)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(policyDecision, NULL);
  return policyDecision->GetAuthPassword();
}

const char* ewk_policy_decision_url_get(Ewk_Policy_Decision* policyDecision)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(policyDecision, NULL);
  return policyDecision->GetUrl();
}

const char* ewk_policy_decision_scheme_get(Ewk_Policy_Decision* policyDecision)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(policyDecision, NULL);
  return policyDecision->GetScheme();
}

const char* ewk_policy_decision_host_get(Ewk_Policy_Decision* policyDecision)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(policyDecision, NULL);
  return policyDecision->GetHost();
}

const char* ewk_policy_decision_http_method_get(Ewk_Policy_Decision* policyDecision)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(policyDecision, NULL);
#if !defined(EWK_BRINGUP)
  return policyDecision->GetHttpMethod();
#else
  return NULL;
#endif
}

Ewk_Policy_Decision_Type ewk_policy_decision_type_get(const Ewk_Policy_Decision* policyDecision)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(policyDecision, EWK_POLICY_DECISION_USE);
  return chromium_glue::from(policyDecision->GetDecisionType());
}

const char* ewk_policy_decision_response_mime_get(const Ewk_Policy_Decision* policyDecision)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(policyDecision, NULL);
  return policyDecision->GetResponseMime();
}

const Eina_Hash* ewk_policy_decision_response_headers_get(const Ewk_Policy_Decision* policyDecision)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(policyDecision, NULL);
  return policyDecision->GetResponseHeaders();
}

int ewk_policy_decision_response_status_code_get(const Ewk_Policy_Decision* policyDecision)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(policyDecision, 0);
  return policyDecision->GetResponseStatusCode();
}

Eina_Bool ewk_policy_decision_suspend(Ewk_Policy_Decision* policyDecision)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(policyDecision, EINA_FALSE);
  policyDecision->Suspend();
  // It is already suspended. We don't need to take any action here.
  return EINA_TRUE;
}

Eina_Bool ewk_policy_decision_use(Ewk_Policy_Decision* policyDecision)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(policyDecision, EINA_FALSE);
  policyDecision->Use();
  return EINA_TRUE;
}

Eina_Bool ewk_policy_decision_ignore(Ewk_Policy_Decision* policyDecision)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(policyDecision, EINA_FALSE);
  policyDecision->Ignore();
  return EINA_TRUE;
}

Eina_Bool ewk_policy_decision_download(Ewk_Policy_Decision* policyDecision)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(policyDecision, EINA_FALSE);
  policyDecision->Download();
  return EINA_TRUE;
}

Ewk_Policy_Navigation_Type ewk_policy_decision_navigation_type_get(const Ewk_Policy_Decision* policyDecision)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(policyDecision, EWK_POLICY_NAVIGATION_TYPE_OTHER);
  return chromium_glue::from(policyDecision->GetNavigationType());
}

Ewk_Frame_Ref ewk_policy_decision_frame_get(Ewk_Policy_Decision* policyDecision)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(policyDecision, NULL);
  return static_cast<Ewk_Frame_Ref>(policyDecision->GetFrameRef());
}

Eina_Bool ewk_policy_decision_is_main_frame(const Ewk_Policy_Decision* policyDecision)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(policyDecision, EINA_FALSE);
#if !defined(EWK_BRINGUP)
  return policyDecision->GetFrameRef()->IsMainFrame() ? EINA_TRUE : EINA_FALSE;
#else
  return EINA_TRUE;
#endif
}

