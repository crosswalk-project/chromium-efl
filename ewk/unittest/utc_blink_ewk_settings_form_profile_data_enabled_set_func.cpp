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

class utc_blink_ewk_settings_form_profile_data_enabled_set : public utc_blink_ewk_base {
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
TEST_F(utc_blink_ewk_settings_form_profile_data_enabled_set, POS_TEST_EINA_TRUE)
{
    ASSERT_EQ(ewk_settings_form_profile_data_enabled_set(settings, EINA_TRUE), EINA_TRUE);
    EXPECT_EQ(ewk_settings_form_profile_data_enabled_get(settings), EINA_TRUE);
}

/**
 * @brief Tests if sets correctly, when called with correct WebSettings object
 */
TEST_F(utc_blink_ewk_settings_form_profile_data_enabled_set, POS_TEST_EINA_FALSE)
{
    ASSERT_EQ(ewk_settings_form_profile_data_enabled_set(settings, EINA_FALSE), EINA_TRUE);
    EXPECT_EQ(ewk_settings_form_profile_data_enabled_get(settings), EINA_FALSE);
}

/**
 * @brief Tests if returns NULL when called with NULL WebSettings object
 */
TEST_F(utc_blink_ewk_settings_form_profile_data_enabled_set, NEG_TEST)
{
    EXPECT_NE(ewk_settings_form_profile_data_enabled_set(NULL, EINA_FALSE), EINA_TRUE);
}
