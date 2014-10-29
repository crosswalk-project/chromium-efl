// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_settings_font_default_size_get : public utc_blink_ewk_base
{
  void PostSetUp()
  {
    settings = ewk_view_settings_get(GetEwkWebView());
    ASSERT_TRUE(settings) << "ewk_view_settings_get(GetEwkWebView())";
  }

protected:
  Ewk_Settings* settings;
};

/**
 * @brief Positive test case for ewk_settings_font_default_size_get().
 */
TEST_F(utc_blink_ewk_settings_font_default_size_get, Default)
{
  EXPECT_NE(0,ewk_settings_font_default_size_get(settings));
}

/**
 * @brief Positive test case for ewk_settings_font_default_size_get().
 *        Check new value
 */
TEST_F(utc_blink_ewk_settings_font_default_size_get, Change)
{
  int size = 1 ^ ewk_settings_font_default_size_get(settings);
  ASSERT_EQ(EINA_TRUE, ewk_settings_font_default_size_set(settings, size));
  EXPECT_EQ(size, ewk_settings_font_default_size_get(settings));
}

/**
 * @brief Negative test case for ewk_settings_font_default_size_get().
 *        Check if API works fine with NULL as settings.
 */
TEST_F(utc_blink_ewk_settings_font_default_size_get, NullArg)
{
  EXPECT_EQ(0, ewk_settings_font_default_size_get(NULL));
}
