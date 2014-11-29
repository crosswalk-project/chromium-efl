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
