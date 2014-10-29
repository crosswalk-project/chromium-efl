// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_settings_javascript_enabled_set : public utc_blink_ewk_base {
};

/**
 * @brief Positive test case of ewk_settings_javascript_enabled_set()
 */
TEST_F(utc_blink_ewk_settings_javascript_enabled_set, POS_TEST1)
{
  Ewk_Settings* settings = ewk_view_settings_get(GetEwkWebView());
  if (!settings) {
    FAIL();
  }

  Eina_Bool result = ewk_settings_javascript_enabled_set(settings, EINA_TRUE);
  if (result == EINA_TRUE) {
    EXPECT_EQ(ewk_settings_javascript_enabled_get(settings), EINA_TRUE);
  }
  else {
    EXPECT_EQ(result, EINA_TRUE);
  }
}

/**
 * @brief Positive test case of ewk_settings_javascript_enabled_set()
 */
TEST_F(utc_blink_ewk_settings_javascript_enabled_set, POS_TEST2)
{
  Ewk_Settings* settings = ewk_view_settings_get(GetEwkWebView());
  if (!settings) {
    FAIL();
  }

  Eina_Bool result = ewk_settings_javascript_enabled_set(settings, EINA_FALSE);
  if (result == EINA_TRUE) {
    EXPECT_EQ(ewk_settings_javascript_enabled_get(settings), EINA_FALSE);
  }
  else {
    EXPECT_EQ(result, EINA_TRUE);
  }
}

/**
 * @brief Negative test case of ewk_settings_javascript_enabled_set()
 */
TEST_F(utc_blink_ewk_settings_javascript_enabled_set, NEG_TEST)
{
  EXPECT_EQ(ewk_settings_javascript_enabled_set(NULL, EINA_FALSE), EINA_FALSE);
}
