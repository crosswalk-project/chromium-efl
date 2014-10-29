// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_settings_text_autosizing_enabled_set_func : public utc_blink_ewk_base
{
protected:
  void PostSetUp(){
    settings = ewk_view_settings_get(GetEwkWebView());
    ASSERT_TRUE(settings != NULL);
  }

  utc_blink_ewk_settings_text_autosizing_enabled_set_func() : settings(NULL){
  }

protected:
  Ewk_Settings* settings;
};


/**
 * @brief Positive test case of ewk_settings_text_autosizing_enabled_set()
 */
TEST_F(utc_blink_ewk_settings_text_autosizing_enabled_set_func, SetTrue)
{
  ASSERT_TRUE(ewk_settings_text_autosizing_enabled_set(settings, EINA_TRUE));
  EXPECT_TRUE(ewk_settings_text_autosizing_enabled_get(settings));
}

/**
 * @brief Positive test case of ewk_settings_text_autosizing_enabled_set()
 */
TEST_F(utc_blink_ewk_settings_text_autosizing_enabled_set_func, SetFlase)
{
  ASSERT_TRUE(ewk_settings_text_autosizing_enabled_set(settings, EINA_FALSE));
  EXPECT_FALSE(ewk_settings_text_autosizing_enabled_get(settings));
}

/**
 * @brief Test case of ewk_settings_text_autosizing_enabled_set()
 */
TEST_F(utc_blink_ewk_settings_text_autosizing_enabled_set_func, InvalidArg)
{
  EXPECT_FALSE(ewk_settings_text_autosizing_enabled_set(NULL, EINA_TRUE));
}
