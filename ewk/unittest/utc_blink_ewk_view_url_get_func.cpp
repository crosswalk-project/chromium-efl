// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

static const char* TEST_URL = "http://www.google.com/";

class utc_blink_ewk_view_url_get : public utc_blink_ewk_base {
};

/**
* @brief Checking whether url is properly set.
 */
TEST_F(utc_blink_ewk_view_url_get, POS_TEST)
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
TEST_F(utc_blink_ewk_view_url_get, NEG_TEST)
{
  const char* currentUrl = ewk_view_url_get(NULL);
  EXPECT_EQ(currentUrl ? EINA_TRUE : EINA_FALSE, EINA_FALSE);
}

