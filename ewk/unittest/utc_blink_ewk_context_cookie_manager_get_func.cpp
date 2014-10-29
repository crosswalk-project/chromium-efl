// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
