// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_settings_font_default_size_set_func : public utc_blink_ewk_base
{
protected:
  utc_blink_ewk_settings_font_default_size_set_func() : settings(NULL){
  }

  void PostSetUp(){
    settings = ewk_view_settings_get(GetEwkWebView());
    ASSERT_TRUE(settings != NULL);
  }

protected:
  Ewk_Settings* settings;
};

/**
 * @brief Positive test case for ewk_settings_font_default_size_set(). Set new size of default font and verify new value.
 */
TEST_F(utc_blink_ewk_settings_font_default_size_set_func, POS_TEST)
{
  int defaultSize = ewk_settings_font_default_size_get(settings);
  ASSERT_NE(defaultSize, 0);

  int const size = defaultSize ^ 1;
  ASSERT_TRUE(ewk_settings_font_default_size_set(settings, size));

  defaultSize = ewk_settings_font_default_size_get(settings);
  EXPECT_EQ(defaultSize, size);
}

/**
 * @brief Negative test case for ewk_settings_font_default_size_set(). Check if API works fine with NULL as settings.
 */
TEST_F(utc_blink_ewk_settings_font_default_size_set_func, InvalidArg)
{
  int const defaultSize = ewk_settings_font_default_size_get(settings);
  EXPECT_FALSE(ewk_settings_font_default_size_set(NULL, defaultSize ^ 1));
}
