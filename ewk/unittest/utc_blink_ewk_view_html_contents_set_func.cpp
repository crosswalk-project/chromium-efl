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


class utc_blink_ewk_view_html_contents_set : public utc_blink_ewk_base
{
protected:
  static const char* const htmlstr;
  static const char* const urlstr;
};

const char* const utc_blink_ewk_view_html_contents_set::htmlstr = "<html><body><div style=\"color:#ff0000\"> this is a test message.</div></body</html>";
const char* const utc_blink_ewk_view_html_contents_set::urlstr = "http://www.google.com";

/**
 * @brief Positive test case of ewk_view_html_contents_set()
 */
TEST_F(utc_blink_ewk_view_html_contents_set, POS_TEST1)
{
  Eina_Bool result = ewk_view_html_contents_set(GetEwkWebView(),htmlstr,urlstr);
  utc_check_eq(result, EINA_TRUE);
}

/**
 * @brief Positive test case of ewk_view_html_contents_set()
 */
TEST_F(utc_blink_ewk_view_html_contents_set, POS_TEST2)
{
  Eina_Bool result = ewk_view_html_contents_set(GetEwkWebView(),NULL,urlstr);
  utc_check_eq(result, EINA_FALSE);
}

/**
 * @brief Positive test case of ewk_view_html_contents_set()
 */
TEST_F(utc_blink_ewk_view_html_contents_set, POS_TEST3)
{
  Eina_Bool result = ewk_view_html_contents_set(GetEwkWebView(),htmlstr,NULL);
  utc_check_eq(result, EINA_TRUE);
}


/**
 * @brief Negative test case of ewk_view_html_contents_set()
 */

TEST_F(utc_blink_ewk_view_html_contents_set, NEG_TEST)
{
  Eina_Bool result = ewk_view_html_contents_set(NULL,htmlstr,urlstr);
  utc_check_ne(result, EINA_TRUE);
}
