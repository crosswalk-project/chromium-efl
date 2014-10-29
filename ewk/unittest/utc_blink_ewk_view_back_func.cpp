// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_view_back : public utc_blink_ewk_base
{
protected:
  /* Callback for load finished */
  void LoadFinished(Evas_Object* webview)
  {
    EventLoopStop(Success);
  }
};

/**
 * @brief Change whether it is possible to go back when two webpages have been loaded
 */
TEST_F(utc_blink_ewk_view_back, POS_TEST)
{
  if(!ewk_view_url_set(GetEwkWebView(),GetResourceUrl("common/sample.html").c_str()))
    utc_fail();
  if(Success!=EventLoopStart())
    utc_fail();

  if(!ewk_view_url_set(GetEwkWebView(),GetResourceUrl("common/sample_1.html").c_str()))
    utc_fail();
  if(Success!=EventLoopStart())
    utc_fail();

  Eina_Bool result = ewk_view_back(GetEwkWebView());
  if(Success!=EventLoopStart())
    utc_fail();

  utc_check_eq(result, EINA_TRUE);
}

/**
 * @brief Checking whether function works properly in case of NULL of a webview.
 */
TEST_F(utc_blink_ewk_view_back, NEG_TEST)
{
  Eina_Bool result = ewk_view_back(NULL);
  utc_check_eq(result, EINA_FALSE);
}
