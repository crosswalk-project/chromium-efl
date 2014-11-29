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

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_view_custom_header_remove : public utc_blink_ewk_base {
protected:
  static const char* test_name;
  static const char* test_value;
  static const char* testnull_name;
  static const char* testnull_value;
  static const char* testnotexisting_name;
};

const char* utc_blink_ewk_view_custom_header_remove::test_name = "TestRemove-Name";
const char* utc_blink_ewk_view_custom_header_remove::test_value = "TestRemove-Value";
const char* utc_blink_ewk_view_custom_header_remove::testnull_name = "TestRemoveNull-Name";
const char* utc_blink_ewk_view_custom_header_remove::testnull_value = "TestRemoveNull-Value";
const char* utc_blink_ewk_view_custom_header_remove::testnotexisting_name = "TestRemoveNotExisting-Name";


/**
 * @brief Checking whether the custom header is removed properly.
 */
TEST_F(utc_blink_ewk_view_custom_header_remove, POS_TEST)
{
  ewk_view_custom_header_add(GetEwkWebView(), test_name, test_value);
  Eina_Bool result = ewk_view_custom_header_remove(GetEwkWebView(), test_name);
  EXPECT_EQ(result, EINA_TRUE);
}

/**
 * @brief Checking whether function works properly in case of NULL of a webview.
 */
TEST_F(utc_blink_ewk_view_custom_header_remove, NEG_TEST1)
{
  ewk_view_custom_header_add(GetEwkWebView(), testnull_name, testnull_value);
  Eina_Bool result = ewk_view_custom_header_remove(NULL, testnull_name);
  EXPECT_NE(result, EINA_TRUE);
}

TEST_F(utc_blink_ewk_view_custom_header_remove, NEG_TEST2)
{
  Eina_Bool result = ewk_view_custom_header_remove(GetEwkWebView(), testnotexisting_name);
  EXPECT_NE(result, EINA_TRUE);
}
