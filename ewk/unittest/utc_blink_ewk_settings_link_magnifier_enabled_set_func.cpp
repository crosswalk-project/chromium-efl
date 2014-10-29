// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_settings_link_magnifier_enabled_set_func : public utc_blink_ewk_base
{
};


/**
 * @brief Tests setting link magnifier enabled to TRUE.
 */
TEST_F(utc_blink_ewk_settings_link_magnifier_enabled_set_func, POS_TEST_TRUE)
{
  Ewk_Settings* settings = ewk_view_settings_get(GetEwkWebView());
  ASSERT_TRUE(settings);

  ewk_settings_link_magnifier_enabled_set(settings, EINA_TRUE);
  ASSERT_TRUE(ewk_settings_link_magnifier_enabled_get(settings));
}

/**
 * @brief Tests setting link magnifier enabled to FALSE.
 */
TEST_F(utc_blink_ewk_settings_link_magnifier_enabled_set_func, POS_TEST_FALSE)
{
  Ewk_Settings* settings = ewk_view_settings_get(GetEwkWebView());
  ASSERT_TRUE(settings);

  ewk_settings_link_magnifier_enabled_set(settings, EINA_FALSE);
  ASSERT_FALSE(ewk_settings_link_magnifier_enabled_get(settings));
}

/**
 * @brief Tests if setting link magnifier enabled with NULL settings fails.
 */
TEST_F(utc_blink_ewk_settings_link_magnifier_enabled_set_func, NEG_TEST)
{

  Ewk_Settings* settings = ewk_view_settings_get(GetEwkWebView());
  ASSERT_TRUE(settings);
  ewk_settings_link_magnifier_enabled_set(settings, EINA_TRUE);
  ASSERT_TRUE(ewk_settings_link_magnifier_enabled_get(settings));

  ewk_settings_link_magnifier_enabled_set(NULL, EINA_FALSE);

  ASSERT_TRUE(ewk_settings_link_magnifier_enabled_get(settings));;
}
