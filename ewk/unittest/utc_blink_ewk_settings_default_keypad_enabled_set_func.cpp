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

class utc_blink_ewk_settings_default_keypad_enabled_set : public utc_blink_ewk_base {
};

/**
 * @brief Tests if returns TRUE when initiated with a correct webview and
 * default keypad enabled.
 */
TEST_F(utc_blink_ewk_settings_default_keypad_enabled_set, Set)
{
  Ewk_Settings* settings = ewk_view_settings_get(GetEwkWebView());
  ASSERT_TRUE(NULL != settings);

  Eina_Bool set = !ewk_settings_default_keypad_enabled_get(settings);
  ASSERT_EQ(EINA_TRUE, ewk_settings_default_keypad_enabled_set(settings, set));
  ASSERT_EQ(set, ewk_settings_default_keypad_enabled_get(settings));

  set = !set;
  ASSERT_EQ(EINA_TRUE, ewk_settings_default_keypad_enabled_set(settings, set));
  ASSERT_EQ(set, ewk_settings_default_keypad_enabled_get(settings));
}

/**
 * @brief Tests if returns FALSE when initiated with NULL webview.
 */
TEST_F(utc_blink_ewk_settings_default_keypad_enabled_set, NullArg)
{
  ASSERT_NE(EINA_TRUE, ewk_settings_default_keypad_enabled_set(NULL, EINA_FALSE));
}
