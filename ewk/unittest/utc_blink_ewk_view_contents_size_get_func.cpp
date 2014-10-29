// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_view_contents_size_get : public utc_blink_ewk_base
{
  /* Callback for load finished */
  void LoadFinished(Evas_Object* webview)
  {
    utc_message("[loadFinished] :: \n");
    EventLoopStop( Success );
  }
};

/**
 * @brief Positive test case of ewk_view_contents_size_get(). Page is loaded and API is called to get content size.
 */
TEST_F(utc_blink_ewk_view_contents_size_get, POS_TEST)
{
  if(!ewk_view_url_set(GetEwkWebView(), GetResourceUrl("common/sample.html").c_str()))
    utc_fail();
  if(Success!=EventLoopStart())
    utc_fail();

  Evas_Coord width = 0;
  Evas_Coord height = 0;

  Eina_Bool result = ewk_view_contents_size_get(GetEwkWebView(), &width, &height);

  if (width == 0 || height == 0)
    result = EINA_FALSE;

  utc_check_eq(result, EINA_TRUE);
}

/**
 * @brief Checking whether function works properly in case of NULL of a webview.
 */
TEST_F(utc_blink_ewk_view_contents_size_get, NEG_TEST)
{
  Evas_Coord width = 0;
  Evas_Coord height = 0;

  Eina_Bool result = ewk_view_contents_size_get(NULL, &width, &height);
  utc_check_eq(result, EINA_FALSE);
}
