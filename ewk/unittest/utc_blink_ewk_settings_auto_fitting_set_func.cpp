// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_settings_auto_fitting_set : public utc_blink_ewk_base {
};


/**
 * @brief Positive test case of ewk_settings_auto_fitting_set()
 */
TEST_F(utc_blink_ewk_settings_auto_fitting_set, POS_TEST1)
{
  Ewk_Settings* settings = ewk_view_settings_get(GetEwkWebView());
  if (!settings) {
    FAIL();
  }
  int resultCheck = ewk_settings_auto_fitting_set(settings, EINA_TRUE);
  if (resultCheck == EINA_TRUE) {
    EXPECT_EQ((int)ewk_settings_auto_fitting_get(settings), 1);
  }
  else {
    EXPECT_EQ(resultCheck, 1);
  }
}

/**
 * @brief Positive test case of ewk_settings_auto_fitting_set()
 */
TEST_F(utc_blink_ewk_settings_auto_fitting_set, POS_TEST2)
{
  Ewk_Settings* settings = ewk_view_settings_get(GetEwkWebView());
  if (!settings) {
    FAIL();
  }
  int resultCheck = ewk_settings_auto_fitting_set(settings, EINA_FALSE);
  if (resultCheck == EINA_TRUE) {
    EXPECT_EQ((int)ewk_settings_auto_fitting_get(settings), 0);
  }
  else {
    EXPECT_EQ(resultCheck, 1);
  }
}

/**
 * @brief Negative test case of ewk_settings_auto_fitting_set()
 */
TEST_F(utc_blink_ewk_settings_auto_fitting_set, NEG_TEST)
{
  int check = ewk_settings_auto_fitting_set(NULL, EINA_TRUE);
  EXPECT_EQ(check, 0);
}
