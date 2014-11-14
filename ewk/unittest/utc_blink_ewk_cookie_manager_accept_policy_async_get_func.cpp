// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_cookie_manager_accept_policy_async_get : public utc_blink_ewk_base
{
protected:
  Ewk_Cookie_Accept_Policy cookies_policy;

  static void cookies_policy_get_cb(Ewk_Cookie_Accept_Policy policy, void* event_info)
  {
    ASSERT_TRUE(event_info);
    utc_blink_ewk_cookie_manager_accept_policy_async_get* owner = static_cast<utc_blink_ewk_cookie_manager_accept_policy_async_get*>(event_info);
    owner->cookies_policy = policy;
    owner->EventLoopStop(utc_blink_ewk_base::Success);
  }
};

TEST_F(utc_blink_ewk_cookie_manager_accept_policy_async_get, positive_test)
{
  cookies_policy = EWK_COOKIE_ACCEPT_POLICY_ALWAYS;

  Ewk_Cookie_Manager* cookieManager = ewk_context_cookie_manager_get(ewk_view_context_get(GetEwkWebView()));
  ASSERT_TRUE(cookieManager);
  ewk_cookie_manager_accept_policy_set(cookieManager, EWK_COOKIE_ACCEPT_POLICY_NEVER);
  ewk_cookie_manager_accept_policy_async_get(cookieManager, cookies_policy_get_cb, this);

  ASSERT_EQ(Success,  EventLoopStart());
  ASSERT_EQ(cookies_policy, EWK_COOKIE_ACCEPT_POLICY_NEVER);
}

TEST_F(utc_blink_ewk_cookie_manager_accept_policy_async_get, invalid_param_callback)
{
  Ewk_Cookie_Manager* cookieManager = ewk_context_cookie_manager_get(ewk_view_context_get(GetEwkWebView()));
  ASSERT_TRUE(cookieManager);
  ewk_cookie_manager_accept_policy_async_get(cookieManager, NULL, this);
}

TEST_F(utc_blink_ewk_cookie_manager_accept_policy_async_get, invalid_param_cookie_manager)
{
  ewk_cookie_manager_accept_policy_async_get(NULL, cookies_policy_get_cb, this);
  ASSERT_EQ(Timeout,  EventLoopStart(3)) << "This callback shall never be called";
}

TEST_F(utc_blink_ewk_cookie_manager_accept_policy_async_get, invalid_param_all)
{
  ewk_cookie_manager_accept_policy_async_get(NULL, NULL, NULL);
}
