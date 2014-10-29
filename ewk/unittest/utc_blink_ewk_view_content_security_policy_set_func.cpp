// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#define SAMPLE_CONTENT_SECURITY_POLICY "ewk_view/sample_content_security_policy.html"

#include "utc_blink_ewk_base.h"

const char* policy = "default-src 'self';";

class utc_blink_ewk_view_content_security_policy_set : public utc_blink_ewk_base {
protected:
  std::string title;

  void LoadFinished(Evas_Object* webview) {

     ecore_timer_add(2, getTitle, this);
  }

  static Eina_Bool getTitle(void *data)
  {

    utc_blink_ewk_view_content_security_policy_set* owner = static_cast<utc_blink_ewk_view_content_security_policy_set*>(data);

    owner->title = ewk_view_title_get(owner->GetEwkWebView());

    owner->EventLoopStop(utc_blink_ewk_base::Success);

    return EINA_FALSE;
  }
};

/**
 * @brief Tests if it isn't possible to eval inline script when content security policy is set
 * and enforce policy header is set.
 */
TEST_F(utc_blink_ewk_view_content_security_policy_set, POS_TEST)
{
  ewk_view_content_security_policy_set(GetEwkWebView(), policy, EWK_ENFORCE_POLICY);

  Eina_Bool result = ewk_view_url_set(GetEwkWebView(), GetResourceUrl(SAMPLE_CONTENT_SECURITY_POLICY).c_str());
  if (!result)
    FAIL();

  utc_blink_ewk_base::MainLoopResult main_result = EventLoopStart();
  if (main_result != utc_blink_ewk_base::Success)
    FAIL();

  utc_check_str_eq((const char*)title.c_str(), "PASS");
}

/**
 * @brief Tests if there is possibility to eval inline script when content security policy
 * and report only policy header is set.
 */
TEST_F(utc_blink_ewk_view_content_security_policy_set, POS_TEST2)
{
  ewk_view_content_security_policy_set(GetEwkWebView(), policy, EWK_REPORT_ONLY);

  Eina_Bool result = ewk_view_url_set(GetEwkWebView(), GetResourceUrl(SAMPLE_CONTENT_SECURITY_POLICY).c_str());
  if (!result)
    FAIL();

  utc_blink_ewk_base::MainLoopResult main_result = EventLoopStart();
  if (main_result != utc_blink_ewk_base::Success)
    FAIL();

  utc_check_str_eq((const char*)title.c_str(), "FAIL");
}

/**
 * @brief Tests if there is possibility to eval inline script when content security policy is disabled.
 */
TEST_F(utc_blink_ewk_view_content_security_policy_set, NEG_TEST)
{
  ewk_view_content_security_policy_set(NULL, NULL, EWK_ENFORCE_POLICY);

  Eina_Bool result = ewk_view_url_set(GetEwkWebView(), GetResourceUrl(SAMPLE_CONTENT_SECURITY_POLICY).c_str());
  if (!result)
    FAIL();

  utc_blink_ewk_base::MainLoopResult main_result = EventLoopStart();
  if (main_result != utc_blink_ewk_base::Success)
    FAIL();

  utc_check_str_ne((const char*)title.c_str(), "PASS");
}
