/*
 * chromium EFL
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

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_cookie_manager_async_accept_policy_get : public utc_blink_ewk_base
{
protected:
  Ewk_Cookie_Accept_Policy cookies_policy;

  static void cookies_policy_get_cb(Ewk_Cookie_Accept_Policy policy, Ewk_Error* error, void* event_info)
  {
    ASSERT_TRUE(event_info);
    utc_blink_ewk_cookie_manager_async_accept_policy_get* owner = static_cast<utc_blink_ewk_cookie_manager_async_accept_policy_get*>(event_info);
    owner->cookies_policy = policy;
    owner->EventLoopStop(utc_blink_ewk_base::Success);
  }
};

TEST_F(utc_blink_ewk_cookie_manager_async_accept_policy_get, positive_test)
{
  cookies_policy = EWK_COOKIE_ACCEPT_POLICY_ALWAYS;

  Ewk_Cookie_Manager* cookieManager = ewk_context_cookie_manager_get(ewk_view_context_get(GetEwkWebView()));
  ASSERT_TRUE(cookieManager);
  ewk_cookie_manager_accept_policy_set(cookieManager, EWK_COOKIE_ACCEPT_POLICY_NEVER);
  ewk_cookie_manager_async_accept_policy_get(cookieManager, cookies_policy_get_cb, this);

  ASSERT_EQ(Success,  EventLoopStart());
  ASSERT_EQ(cookies_policy, EWK_COOKIE_ACCEPT_POLICY_NEVER);
}

TEST_F(utc_blink_ewk_cookie_manager_async_accept_policy_get, invalid_param_callback)
{
  Ewk_Cookie_Manager* cookieManager = ewk_context_cookie_manager_get(ewk_view_context_get(GetEwkWebView()));
  ASSERT_TRUE(cookieManager);
  ewk_cookie_manager_async_accept_policy_get(cookieManager, NULL, this);
}

TEST_F(utc_blink_ewk_cookie_manager_async_accept_policy_get, invalid_param_cookie_manager)
{
  ewk_cookie_manager_async_accept_policy_get(NULL, cookies_policy_get_cb, this);
  ASSERT_EQ(Failure,  EventLoopStart(3)) << "This callback shall never be called";
}

TEST_F(utc_blink_ewk_cookie_manager_async_accept_policy_get, invalid_param_all)
{
  ewk_cookie_manager_async_accept_policy_get(NULL, NULL, NULL);
}
