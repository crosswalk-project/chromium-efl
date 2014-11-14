// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/* Define those macros _before_ you include the utc_blink_ewk.h header file. */

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_cookie_manager_accept_policy_set : public utc_blink_ewk_base
{
protected:
  Ewk_Cookie_Accept_Policy cookies_policy;

  static void cookies_policy_get_cb(Ewk_Cookie_Accept_Policy policy, void *event_info)
  {
    utc_blink_ewk_cookie_manager_accept_policy_set *owner = static_cast<utc_blink_ewk_cookie_manager_accept_policy_set*>(event_info);
    owner->cookies_policy = policy;
    owner->EventLoopStop(utc_blink_ewk_base::Success);
  }
};

/**
 * @brief Checking whether cookies's policy is returned to EWK_COOKIE_ACCEPT_POLICY_NEVER.
 */
TEST_F(utc_blink_ewk_cookie_manager_accept_policy_set, POS_TEST1)
{
  cookies_policy = EWK_COOKIE_ACCEPT_POLICY_ALWAYS;

  Ewk_Cookie_Manager* cookieManager = ewk_context_cookie_manager_get(ewk_view_context_get(GetEwkWebView()));
  ewk_cookie_manager_accept_policy_set(cookieManager, EWK_COOKIE_ACCEPT_POLICY_NEVER);
  ewk_cookie_manager_accept_policy_async_get(cookieManager, cookies_policy_get_cb, this);

  // Wait until callback finished.
  utc_blink_ewk_base::MainLoopResult main_result = EventLoopStart();
  if (main_result != utc_blink_ewk_base::Success)
    FAIL();

  utc_check_eq(cookies_policy, EWK_COOKIE_ACCEPT_POLICY_NEVER);
}

/**
 * @brief Checking whether cookies's policy is returned to EWK_COOKIE_ACCEPT_POLICY_ALWAYS.
 */
TEST_F(utc_blink_ewk_cookie_manager_accept_policy_set, POS_TEST2)
{
  cookies_policy = EWK_COOKIE_ACCEPT_POLICY_NEVER;

  Ewk_Cookie_Manager* cookieManager = ewk_context_cookie_manager_get(ewk_view_context_get(GetEwkWebView()));
  ewk_cookie_manager_accept_policy_set(cookieManager, EWK_COOKIE_ACCEPT_POLICY_ALWAYS);
  ewk_cookie_manager_accept_policy_async_get(cookieManager, cookies_policy_get_cb, this);

  // Wait until callback finished.
  utc_blink_ewk_base::MainLoopResult main_result = EventLoopStart();
  if (main_result != utc_blink_ewk_base::Success)
    FAIL();

  utc_check_eq(cookies_policy, EWK_COOKIE_ACCEPT_POLICY_ALWAYS);
}

/**
 * @brief Checking whether cookies's policy is returned to EWK_COOKIE_ACCEPT_POLICY_NO_THIRD_PARTY.
 */
TEST_F(utc_blink_ewk_cookie_manager_accept_policy_set, POS_TEST3)
{
  cookies_policy = EWK_COOKIE_ACCEPT_POLICY_NEVER;

  Ewk_Cookie_Manager* cookieManager = ewk_context_cookie_manager_get(ewk_view_context_get(GetEwkWebView()));
  ewk_cookie_manager_accept_policy_set(cookieManager, EWK_COOKIE_ACCEPT_POLICY_NO_THIRD_PARTY);
  ewk_cookie_manager_accept_policy_async_get(cookieManager, cookies_policy_get_cb, this);

  // Wait until callback finished.
  utc_blink_ewk_base::MainLoopResult main_result = EventLoopStart();
  if (main_result != utc_blink_ewk_base::Success)
    FAIL();

  utc_check_eq(cookies_policy, EWK_COOKIE_ACCEPT_POLICY_NO_THIRD_PARTY);
}

/**
 * @brief Checking whether function works properly in case of NULL of a context.
 */
TEST_F(utc_blink_ewk_cookie_manager_accept_policy_set, NEG_TEST1)
{
  ewk_cookie_manager_accept_policy_set(NULL, EWK_COOKIE_ACCEPT_POLICY_NEVER);
  ewk_cookie_manager_accept_policy_async_get(NULL, NULL, NULL);
}

/**
 * @brief Checking whether function works properly in case of NULL of a context.
 */
TEST_F(utc_blink_ewk_cookie_manager_accept_policy_set, NEG_TEST2)
{
  ewk_cookie_manager_accept_policy_set(NULL, EWK_COOKIE_ACCEPT_POLICY_ALWAYS);
  ewk_cookie_manager_accept_policy_async_get(NULL, NULL, NULL);
}

/**
 * @brief Checking whether function works properly in case of NULL of a context.
 */
TEST_F(utc_blink_ewk_cookie_manager_accept_policy_set, NEG_TEST3)
{
  ewk_cookie_manager_accept_policy_set(NULL, EWK_COOKIE_ACCEPT_POLICY_NO_THIRD_PARTY);
  ewk_cookie_manager_accept_policy_async_get(NULL, NULL, NULL);
}
