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
