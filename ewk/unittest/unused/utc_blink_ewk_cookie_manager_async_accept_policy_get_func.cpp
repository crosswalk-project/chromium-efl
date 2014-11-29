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

/* Define those macros _before_ you include the utc_blink_ewk.h header file. */

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_cookie_manager_async_accept_policy_get : public utc_blink_ewk_base
{
protected:

  static void cookies_policy_get_cb(Ewk_Cookie_Accept_Policy policy, Ewk_Error *error, void *event_info)
  {
    utc_blink_ewk_cookie_manager_async_accept_policy_get *owner = static_cast<utc_blink_ewk_cookie_manager_async_accept_policy_get*>(event_info);
    owner->EventLoopStop(utc_blink_ewk_base::Success);
  }
};

/**
 * @brief Checking whether cookies's policy is returned to EWK_COOKIE_ACCEPT_POLICY_NEVER.
 */
TEST_F(utc_blink_ewk_cookie_manager_async_accept_policy_get, POS_TEST)
{
  Ewk_Cookie_Manager* cookieManager = ewk_context_cookie_manager_get(ewk_view_context_get(GetEwkWebView()));
  ewk_cookie_manager_async_accept_policy_get(cookieManager, cookies_policy_get_cb, this);

  // Wait until callback finished.
  utc_blink_ewk_base::MainLoopResult main_result = EventLoopStart(10.0);

  utc_check_eq(main_result, utc_blink_ewk_base::Success);
}

/**
 * @brief Checking whether function works properly in case of NULL of a context.
 */
TEST_F(utc_blink_ewk_cookie_manager_async_accept_policy_get, NEG_TEST)
{
  ewk_cookie_manager_async_accept_policy_get(NULL, cookies_policy_get_cb, this);

  utc_blink_ewk_base::MainLoopResult main_result = EventLoopStart(5.0);

  utc_check_ne(main_result, utc_blink_ewk_base::Success);
}
