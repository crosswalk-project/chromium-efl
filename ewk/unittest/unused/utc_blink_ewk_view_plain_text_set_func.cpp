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

#include "utc_blink_ewk.h"

extern struct Ewk_Test_View test_view;

class utc_blink_ewk_view_plain_text_set : public ::testing::Test
{
 protected:
  utc_blink_ewk_view_plain_text_set()
  {
  }

  ~utc_blink_ewk_view_plain_text_set()
  {
  }

  void SetUp()
  {
    utc_blink_ewk_test_init();
  }

  void TearDown()
  {
    utc_blink_ewk_test_end();
  }

  /**
  * @brief Positive test case of ewk_view_plain_text_set(), checking whether text is set properly.
  */
  static void plain_text_get_cb(Evas_Object* o, const char* plain_text, void* user_data)
  {
    utc_check_str_eq("This is Plain Text", plain_text);
  }
};

/**
* @brief Positive test case of ewk_view_plain_text_set().Checking whether function works properly
*/
TEST_F(utc_blink_ewk_view_plain_text_set, POS_TEST)
{
  utc_check_eq(ewk_view_plain_text_set(test_view.webview,"This is Plain Text"), EINA_TRUE);
  ewk_view_plain_text_get(test_view.webview, plain_text_get_cb, NULL);
}

/**
* @brief Checking whether function works properly in case of NULL of command.
*/
TEST_F(utc_blink_ewk_view_plain_text_set, NEG_TEST)
{
  utc_check_eq(ewk_view_plain_text_set(NULL,"This is Negative Plain Text"), EINA_FALSE);
}
