// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_view_scroll_pos_get : public utc_blink_ewk_base
{
 protected:
  static void frameRendered(void* data, Evas_Object* eObject, void* dataFinished)
  {
    utc_message("[frameRendered] :: \n");
    if(data)
      static_cast<utc_blink_ewk_view_scroll_pos_get*>(data)->EventLoopStop(Success);
  }

  /* Startup function */
  void PostSetUp()
  {
    evas_object_smart_callback_add(GetEwkWebView(), "frame,rendered", frameRendered, this);
  }

  /* Cleanup function */
  void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(), "frame,rendered", frameRendered);
  }

protected:
  static const char*const resource;
};

const char*const utc_blink_ewk_view_scroll_pos_get::resource="/ewk_view/index_big_red_square.html";

/**
 * @brief Positive test case of ewk_view_scroll_pos_get(),tseting scroll postion against after set, and scroll
 */
TEST_F(utc_blink_ewk_view_scroll_pos_get, POS_TEST1)
{
  if(!ewk_view_url_set(GetEwkWebView(),GetResourceUrl(resource).c_str()))
     utc_fail();
  if(Success!=EventLoopStart())
    utc_fail();
  int x = 0, y = 0;
  const int valScrollSet = 2;

  ewk_view_scroll_set(GetEwkWebView(), valScrollSet, valScrollSet);

  ewk_view_scroll_pos_get(GetEwkWebView(), &x, &y);

  Eina_Bool result = EINA_FALSE;
  if (x == valScrollSet && y == valScrollSet)
    result = EINA_TRUE;
}

/**
 * @brief Positive test case of ewk_view_scroll_pos_get(),tseting scroll postion against after set, and scroll
 */
TEST_F(utc_blink_ewk_view_scroll_pos_get, POS_TEST2)
{
  int x = 0, y = 0;
  const int valScrollSet = -12 ;

  ewk_view_scroll_set(GetEwkWebView(), valScrollSet, valScrollSet);

  ewk_view_scroll_pos_get(GetEwkWebView(), &x, &y);

  Eina_Bool result = EINA_FALSE;
  if (x == 0 && y == 0)
    result = EINA_TRUE;

  utc_check_eq(result, EINA_TRUE);
}

/**
 * @brief Negative test case of ewk_view_scroll_pos_get(), testing for null
 */
TEST_F(utc_blink_ewk_view_scroll_pos_get, NEG_TEST)
{
  int x,y;
  Eina_Bool result = ewk_view_scroll_pos_get(NULL, &x, &y);
  utc_check_eq(result, EINA_FALSE);
}
