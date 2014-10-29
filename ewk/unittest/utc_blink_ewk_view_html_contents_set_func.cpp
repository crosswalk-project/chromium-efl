// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
