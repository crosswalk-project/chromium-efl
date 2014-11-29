/*
 * Chromium EFL
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

// #ifndef LegacyFontSizeModes
// #define LegacyFontSizeModes
// enum _Ewk_Legacy_Font_Size_Mode {
//     EWK_LEGACY_FONT_SIZE_MODE_ALWAYS,
//     EWK_LEGACY_FONT_SIZE_MODE_ONLY_IF_PIXEL_VALUES_MATCH,
//     EWK_LEGACY_FONT_SIZE_MODE_NEVER
// };
// #endif //LegacyFontSizeModes

//typedef enum _Ewk_Legacy_Font_Size_Mode Ewk_Legacy_Font_Size_Mode;

class utc_blink_ewk_settings_current_legacy_font_size_mode_set : public utc_blink_ewk_base {
};


/**
* @brief Tests if returns TRUE when initiated with a correct webview and
* "Legacy Font Size Mode" is set to EWK_LEGACY_FONT_SIZE_MODE_ALWAYS.
*/
TEST_F(utc_blink_ewk_settings_current_legacy_font_size_mode_set, POS_TEST1)
{
  Ewk_Settings* settings = ewk_view_settings_get(GetEwkWebView());
  if (!settings) {
    FAIL();
  }

  Eina_Bool result = ewk_settings_current_legacy_font_size_mode_set(settings, EWK_LEGACY_FONT_SIZE_MODE_ALWAYS);
  if (!result) {
    FAIL();
  }
  EXPECT_EQ(result, EINA_TRUE);
}

/**
* @brief Tests if returns TRUE when initiated with a correct webview and
* "Legacy Font Size Mode" is set to EWK_LEGACY_FONT_SIZE_MODE_NEVER.
*/
TEST_F(utc_blink_ewk_settings_current_legacy_font_size_mode_set, POS_TEST2)
{
  Ewk_Settings* settings = ewk_view_settings_get(GetEwkWebView());
  if (!settings) {
    FAIL();
  }

  Eina_Bool result = ewk_settings_current_legacy_font_size_mode_set(settings, EWK_LEGACY_FONT_SIZE_MODE_NEVER);
  if (!result) {
    FAIL();
  }
  EXPECT_EQ(result, EINA_TRUE);
}

/**
* @brief Tests if returns TRUE when initiated with a correct webview and
* "Legacy Font Size Mode" is set to EWK_LEGACY_FONT_SIZE_MODE_ONLY_IF_PIXEL_VALUES_MATCH.
*/
TEST_F(utc_blink_ewk_settings_current_legacy_font_size_mode_set, POS_TEST3)
{
  Ewk_Settings* settings = ewk_view_settings_get(GetEwkWebView());
  if (!settings) {
    FAIL();
  }
  Eina_Bool result = ewk_settings_current_legacy_font_size_mode_set(settings, EWK_LEGACY_FONT_SIZE_MODE_ONLY_IF_PIXEL_VALUES_MATCH);
  if (!result) {
    FAIL();
  }
  EXPECT_EQ(result, EINA_TRUE);
}

/**
* @brief Tests if returns EINA_FALSE when initiated with NULL webview.
*/
TEST_F(utc_blink_ewk_settings_current_legacy_font_size_mode_set, NEG_TEST)
{
  Eina_Bool result = ewk_settings_current_legacy_font_size_mode_set(NULL, EWK_LEGACY_FONT_SIZE_MODE_ALWAYS);
  EXPECT_EQ(result, EINA_FALSE);
}
