/*
 * Chromium EFL
 *
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

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
