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

/* Define those macros _before_ you include the utc_blink_ewk.h header file. */

#include "utc_blink_ewk.h"

extern struct Ewk_Test_View test_view;

typedef enum _Ewk_Legacy_Font_Size_Mode Ewk_Legacy_Font_Size_Mode;

class utc_blink_ewk_settings_current_legacy_font_size_mode_get : public ::testing::Test {
  protected:
    utc_blink_ewk_settings_current_legacy_font_size_mode_get() {
    }

    virtual ~utc_blink_ewk_settings_current_legacy_font_size_mode_get() {
    }

    virtual void SetUp() {
      utc_blink_ewk_test_init();
    }

    virtual void TearDown() {
      utc_blink_ewk_test_end();
    }
};

/**
* @brief Tests if returns EWK_LEGACY_FONT_SIZE_MODE_ALWAYS when initiated with a correct webview and
* "Legacy Font Size Mode" is set to EWK_LEGACY_FONT_SIZE_MODE_ALWAYS.
*/
TEST_F(utc_blink_ewk_settings_current_legacy_font_size_mode_get, POS_TEST1)
{
  Ewk_Settings* settings = ewk_view_settings_get(test_view.webview);
  Eina_Bool result1 =
      ewk_settings_current_legacy_font_size_mode_set(settings, EWK_LEGACY_FONT_SIZE_MODE_ALWAYS);
  if (!result1)
   utc_fail();
  Ewk_Legacy_Font_Size_Mode result2 = ewk_settings_current_legacy_font_size_mode_get(settings);

  utc_check_eq(result2, EWK_LEGACY_FONT_SIZE_MODE_ALWAYS);
}

/**
* @brief Tests if returns EWK_LEGACY_FONT_SIZE_MODE_NEVER when initiated with a correct webview and
* "Legacy Font Size Mode" is set to EWK_LEGACY_FONT_SIZE_MODE_NEVER.
*/
TEST_F(utc_blink_ewk_settings_current_legacy_font_size_mode_get, POS_TEST2)
{
  Ewk_Settings* settings = ewk_view_settings_get(test_view.webview);
  Eina_Bool result =
      ewk_settings_current_legacy_font_size_mode_set(settings, EWK_LEGACY_FONT_SIZE_MODE_NEVER);
  if (!result)
   utc_fail();
  Ewk_Legacy_Font_Size_Mode result2 = ewk_settings_current_legacy_font_size_mode_get(settings);

  utc_check_eq(result2, EWK_LEGACY_FONT_SIZE_MODE_NEVER);
}

/**
* @brief Tests if returns EWK_LEGACY_FONT_SIZE_MODE_ONLY_IF_PIXEL_VALUES_MATCH when initiated with a correct webview and
* "Legacy Font Size Mode" is set to EWK_LEGACY_FONT_SIZE_MODE_ONLY_IF_PIXEL_VALUES_MATCH.
*/
TEST_F(utc_blink_ewk_settings_current_legacy_font_size_mode_get, POS_TEST3)
{
  Ewk_Settings* settings = ewk_view_settings_get(test_view.webview);
  Eina_Bool result =
      ewk_settings_current_legacy_font_size_mode_set(settings, EWK_LEGACY_FONT_SIZE_MODE_ONLY_IF_PIXEL_VALUES_MATCH);
  if (!result)
   utc_fail();
  Ewk_Legacy_Font_Size_Mode result2 = ewk_settings_current_legacy_font_size_mode_get(settings);

  utc_check_eq(result2, EWK_LEGACY_FONT_SIZE_MODE_ONLY_IF_PIXEL_VALUES_MATCH);
}

/**
* @brief Tests if returns EWK_LEGACY_FONT_SIZE_MODE_ONLY_IF_PIXEL_VALUES_MATCH when initiated with NULL webview.
*/
TEST_F(utc_blink_ewk_settings_current_legacy_font_size_mode_get, NEG_TEST)
{
  Ewk_Legacy_Font_Size_Mode result =
      ewk_settings_current_legacy_font_size_mode_get(NULL);
  utc_check_eq(result, EWK_LEGACY_FONT_SIZE_MODE_ONLY_IF_PIXEL_VALUES_MATCH);
}