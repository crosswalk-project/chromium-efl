// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_view_back_possible : public utc_blink_ewk_base
{
protected:
  /* Callback for load finished */
  void LoadFinished(Evas_Object* webview)
  {
    EventLoopStop(Success);
  }
};

/**
 * @brief Check whether it is possible to go back when no webpage is loaded
 */
TEST_F(utc_blink_ewk_view_back_possible, POS_TEST1)
{
  int result = ewk_view_back_possible( GetEwkWebView());
  utc_check_eq(result, 0);
}

/**
 * @brief Change whether it is possible to go back when only one webpage is loaded
 */
TEST_F(utc_blink_ewk_view_back_possible, POS_TEST2)
{
  if(!ewk_view_url_set(GetEwkWebView(),GetResourceUrl("common/sample.html").c_str()))
    utc_fail();
  if(Success!=EventLoopStart())
    utc_fail();

  int result = ewk_view_back_possible( GetEwkWebView());
  utc_check_eq(result, 0);
}

/**
 * @brief Change whether it is possible to go back when two webpages have been loaded
 */
TEST_F(utc_blink_ewk_view_back_possible, POS_TEST3)
{
  if(!ewk_view_url_set(GetEwkWebView(),GetResourceUrl("common/sample.html").c_str()))
    utc_fail();
  if(Success!=EventLoopStart())
    utc_fail();

  if(!ewk_view_url_set(GetEwkWebView(),GetResourceUrl("common/sample_1.html").c_str()))
    utc_fail();
  if(Success!=EventLoopStart())
    utc_fail();

  int result = ewk_view_back_possible( GetEwkWebView());
  utc_check_eq(result, 1);
}

/**
 * @brief Checking whether function works properly in case of NULL of a webview.
 */
TEST_F(utc_blink_ewk_view_back_possible, NEG_TEST)
{
  int result = ewk_view_back_possible(NULL);
  utc_check_eq(result, 0);
}
