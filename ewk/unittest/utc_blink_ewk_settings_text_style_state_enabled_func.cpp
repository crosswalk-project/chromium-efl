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

class utc_blink_ewk_settings_text_style_state_enabled : public utc_blink_ewk_base {
};

/**
 * @brief Tests if returns TRUE when initiated with a correct webview and
 * enabled text selection set to TRUE.
 */
TEST_F(utc_blink_ewk_settings_text_style_state_enabled, POS_TEST1)
{
  Ewk_Settings* settings = ewk_view_settings_get(GetEwkWebView());
  if (!settings) {
    FAIL();
  }

  ewk_settings_text_style_state_enabled_set(settings, EINA_TRUE);
  Eina_Bool result = ewk_settings_text_style_state_enabled_get(settings);
  if (!result) {
    FAIL();
  }
  EXPECT_EQ(result, EINA_TRUE);
}

/**
 * @brief Tests if returns TRUE when initiated with a correct webview and
 * enabled text selection set to FALSE.
 */
TEST_F(utc_blink_ewk_settings_text_style_state_enabled, POS_TEST2)
{
  Ewk_Settings* settings = ewk_view_settings_get(GetEwkWebView());
  if (!settings) {
    FAIL();
  }

  ewk_settings_text_style_state_enabled_set(settings, EINA_FALSE);
  Eina_Bool result = ewk_settings_text_style_state_enabled_get(settings);
  if (result) {
    FAIL();
  }
  EXPECT_EQ(result, EINA_FALSE);
}

/**
 * @brief Tests if returns FALSE when initiated with NULL webview.
 */
TEST_F(utc_blink_ewk_settings_text_style_state_enabled, NEG_TEST)
{
  ewk_settings_text_style_state_enabled_set(NULL, EINA_FALSE);
  SUCCEED();
}
