// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_settings_scripts_window_open_set : public utc_blink_ewk_base {
};

/**
 * @brief Positive test case of ewk_settings_scripts_window_open_set()
 */
TEST_F(utc_blink_ewk_settings_scripts_window_open_set, SetTrue)
{
  Ewk_Settings *const settings = ewk_view_settings_get(GetEwkWebView());
  ASSERT_TRUE(NULL != settings);
  ASSERT_TRUE(ewk_settings_scripts_window_open_set(settings, EINA_TRUE));
  EXPECT_TRUE(ewk_settings_scripts_window_open_get(settings));
}

/**
 * @brief Positive test case of ewk_settings_scripts_window_open_set()
 */
TEST_F(utc_blink_ewk_settings_scripts_window_open_set, SetFalse)
{
  Ewk_Settings *const settings = ewk_view_settings_get(GetEwkWebView());
  ASSERT_TRUE(NULL != settings);
  ASSERT_TRUE(ewk_settings_scripts_window_open_set(settings, EINA_FALSE));
  EXPECT_FALSE(ewk_settings_scripts_window_open_get(settings));
}

/**
 * @brief Test case of ewk_settings_scripts_window_open_set() when view is NULL
 */
TEST_F(utc_blink_ewk_settings_scripts_window_open_set, InvalidArg)
{
  EXPECT_FALSE(ewk_settings_scripts_window_open_set(NULL, EINA_TRUE));
}
