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

class utc_blink_ewk_context_cookie_manager_get : public utc_blink_ewk_base
{
};

/**
 * @brief Positive test case of ewk_context_cookie_manager_get()
 */
TEST_F(utc_blink_ewk_context_cookie_manager_get, POS_TEST)
{
  Ewk_Cookie_Manager* cookieManager = ewk_context_cookie_manager_get(ewk_view_context_get(GetEwkWebView()));
  if (cookieManager)
    utc_pass();
  else
    utc_fail();
}

/**
 * @brief  checking whether ewk_context_cookie_manager_get return NULL when passing NULL value.
 */
TEST_F(utc_blink_ewk_context_cookie_manager_get, NEG_TEST)
{
  Ewk_Cookie_Manager* cookieManager = ewk_context_cookie_manager_get(NULL);
  if (!cookieManager)
    utc_pass();
  else
    utc_fail();
}
