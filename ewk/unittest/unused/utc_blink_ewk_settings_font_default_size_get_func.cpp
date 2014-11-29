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

class utc_blink_ewk_settings_font_default_size_get : public ::testing::Test {
 protected:
  utc_blink_ewk_settings_font_default_size_get() {
  }

  virtual ~utc_blink_ewk_settings_font_default_size_get() {
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
 * @brief Positive test case for ewk_settings_font_default_size_get(). Get the default font size.
 */
TEST_F(utc_blink_ewk_settings_font_default_size_get, POS_TEST1)
{
  Ewk_Settings* settings = ewk_view_settings_get(test_view.webview);
  int defaultSize = 0;
  defaultSize = ewk_settings_font_default_size_get(settings);

  utc_check_ne(defaultSize, 0);
}

/**
 * @brief Positive test case for ewk_settings_font_default_size_get(). Set the default font size and then compare the value.
 */
TEST_F(utc_blink_ewk_settings_font_default_size_get, POS_TEST2)
{
  Ewk_Settings* settings = ewk_view_settings_get(test_view.webview);

  int defaultSize = 0;
  defaultSize = ewk_settings_font_default_size_get(settings);
  if (defaultSize == 0)
    utc_fail();

  int size = defaultSize + 1;
  Eina_Bool result = ewk_settings_font_default_size_set(settings, size);
  if(!result)
    utc_fail();

  defaultSize = ewk_settings_font_default_size_get(settings);
  utc_check_eq(defaultSize, size);
}

/**
 * @brief Negative test case for ewk_settings_font_default_size_get(). Check if API works fine with NULL as settings.
 */
TEST_F(utc_blink_ewk_settings_font_default_size_get, NEG_TEST)
{
  int defaultSize = ewk_settings_font_default_size_get(NULL);
  utc_check_eq(defaultSize, 0);
}
