// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_settings_form_profile_data_enabled_get : public utc_blink_ewk_base
{
public:
  Ewk_Settings* settings;

protected:
  void PostSetUp() {
    settings = ewk_view_settings_get(GetEwkWebView());
    EXPECT_TRUE(settings);
  }
};

/**
 * @brief Tests if sets correctly, when called with correct WebSettings object
 */
TEST_F(utc_blink_ewk_settings_form_profile_data_enabled_get, POS_TEST_EINA_TRUE)
{
    ASSERT_EQ(ewk_settings_form_profile_data_enabled_set(settings, EINA_TRUE), EINA_TRUE);
    EXPECT_EQ(ewk_settings_form_profile_data_enabled_get(settings), EINA_TRUE);
}

/**
 * @brief Tests if sets correctly, when called with correct WebSettings object
 */
TEST_F(utc_blink_ewk_settings_form_profile_data_enabled_get, POS_TEST_EINA_FALSE)
{
    ASSERT_EQ(ewk_settings_form_profile_data_enabled_set(settings, EINA_FALSE), EINA_TRUE);
    EXPECT_EQ(ewk_settings_form_profile_data_enabled_get(settings), EINA_FALSE);
}

/**
 * @brief Tests if returns NULL when called with NULL WebSettings object
 */
TEST_F(utc_blink_ewk_settings_form_profile_data_enabled_get, NEG_TEST)
{
    EXPECT_FALSE(ewk_settings_form_profile_data_enabled_get(NULL));
}

