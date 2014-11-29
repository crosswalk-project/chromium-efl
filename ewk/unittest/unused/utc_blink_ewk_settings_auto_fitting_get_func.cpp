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

#ifndef NULL
#define NULL    0x0
#endif

extern struct Ewk_Test_View test_view;

class utc_blink_ewk_settings_auto_fitting_get : public ::testing::Test {
 protected:
  utc_blink_ewk_settings_auto_fitting_get() {
  }

  virtual ~utc_blink_ewk_settings_auto_fitting_get() {
  }

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
 * @brief Positive test case of ewk_settings_auto_fitting_get()
 */
TEST_F(utc_blink_ewk_settings_auto_fitting_get, POS_TEST1)
{
  Ewk_Settings* settings = ewk_view_settings_get(test_view.webview);
  ewk_settings_auto_fitting_set(settings,EINA_TRUE);
  int result = ewk_settings_auto_fitting_get(settings);
  utc_check_eq(result, 1);
}

/**
 * @brief Positive test case of ewk_settings_auto_fitting_get()
 */
TEST_F(utc_blink_ewk_settings_auto_fitting_get, POS_TEST2)
{
  Ewk_Settings* settings = ewk_view_settings_get(test_view.webview);
  ewk_settings_auto_fitting_set(settings,EINA_FALSE);
  int result = ewk_settings_auto_fitting_get(settings);
  utc_check_eq(result, 0);
}


/**
 * @brief Negative test case of ewk_settings_auto_fitting_get()
 */
TEST_F(utc_blink_ewk_settings_auto_fitting_get, NEG_TEST)
{
  int result = ewk_settings_auto_fitting_get(NULL);
  utc_check_eq(result, 0);
}
