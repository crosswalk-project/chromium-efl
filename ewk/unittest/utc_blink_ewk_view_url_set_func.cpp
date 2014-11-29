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

static const char* TEST_URL = "http://www.google.com/";

class utc_blink_ewk_view_url_set : public utc_blink_ewk_base {
};

/**
* @brief Checking whether url is properly set.
 */
TEST_F(utc_blink_ewk_view_url_set, POS_TEST)
{
  Eina_Bool result = ewk_view_url_set(GetEwkWebView(), TEST_URL);
  if (!result) {
    FAIL();
  }

  const char* currentUrl = ewk_view_url_get(GetEwkWebView());
  EXPECT_STREQ(currentUrl, TEST_URL);
}

/**
* @brief Checking whether function works properly in case of NULL of a webview.
 */
TEST_F(utc_blink_ewk_view_url_set, NEG_TEST1)
{
  Eina_Bool result = ewk_view_url_set(NULL, TEST_URL);
  if (result) {
    FAIL();
  }
}

TEST_F(utc_blink_ewk_view_url_set, NEG_TEST2)
{
  Eina_Bool result = ewk_view_url_set(GetEwkWebView(), NULL);
  if (result) {
    FAIL();
  }
}

