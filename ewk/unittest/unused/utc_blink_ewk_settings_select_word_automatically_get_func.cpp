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

/* Define those macros _before_ you include the utc_blink_ewk.h header file. */
#include "utc_blink_ewk.h"

extern struct Ewk_Test_View test_view;

class utc_blink_ewk_settings_select_word_automatically_get : public ::testing::Test {
 protected:
  utc_blink_ewk_settings_select_word_automatically_get() {
  }

  virtual ~utc_blink_ewk_settings_select_word_automatically_get() {
  }

  /* Startup and cleanup functions */
  virtual void SetUp()
  {
    utc_blink_ewk_test_init();
  }

  virtual void TearDown()
  {
    utc_blink_ewk_test_end();
  }
};

/**
 * @brief Tests if returns TRUE when initiated with a correct webview and
 * enabled select word automatically set to TRUE.
 */
TEST_F(utc_blink_ewk_settings_select_word_automatically_get, POS_TEST1)
{
  Ewk_Settings* settings = ewk_view_settings_get(test_view.webview);
  Eina_Bool result = ewk_settings_select_word_automatically_set(settings, EINA_TRUE);
  if (!result)
    utc_fail();

  result = ewk_settings_select_word_automatically_get(settings);
  utc_check_eq(result, EINA_TRUE);
}

/**
 * @brief Tests if returns FALSE when initiated with a correct webview and
 * enabled select word automatically set to FALSE.
 */
TEST_F(utc_blink_ewk_settings_select_word_automatically_get, POS_TEST2)
{
  Ewk_Settings* settings = ewk_view_settings_get(test_view.webview);
  Eina_Bool result = ewk_settings_select_word_automatically_set(settings, EINA_FALSE);
  if (!result)
    utc_fail();

  result = ewk_settings_select_word_automatically_get(settings);
  utc_check_eq(result, EINA_FALSE);
}

/**
 * @brief Tests if returns FALSE when initiated with NULL webview.
 */
TEST_F(utc_blink_ewk_settings_select_word_automatically_get, NEG_TEST)
{
  Eina_Bool result = ewk_settings_select_word_automatically_get(NULL);
  utc_check_ne(result, EINA_TRUE);
}
