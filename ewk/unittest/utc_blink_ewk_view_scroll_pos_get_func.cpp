// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_view_scroll_pos_get : public utc_blink_ewk_base
{
 protected:
  void LoadFinished(Evas_Object*)
  {
    evas_object_focus_set(GetEwkWebView(), EINA_TRUE);
    EventLoopStop(Success);
  }

  static Eina_Bool quitMainLoop(void* data)
  {
    if (data)
      static_cast<utc_blink_ewk_view_scroll_pos_get*>(data)->EventLoopStop(Success);
    return EINA_FALSE;
  }

protected:
  static const char* resource;
};

const char* utc_blink_ewk_view_scroll_pos_get::resource="/ewk_view/index_big_red_square.html";

/**
 * @brief Positive test case of ewk_view_scroll_pos_get(), setting scroll position against after set, and scroll
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
  ASSERT_TRUE(result);
}

/**
 * @brief Positive test case of ewk_view_scroll_pos_get(), setting scroll position against after set, and scroll
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
 * @brief ewk_view_scroll_pos_get() should return the same value as it has been set if main loop wasn't fired
 */
TEST_F(utc_blink_ewk_view_scroll_pos_get, POS_TEST3)
{
  int x = 0, y = 0;

  ASSERT_TRUE(ewk_view_url_set(GetEwkWebView(),GetResourceUrl(resource).c_str()));
  ASSERT_EQ(Success, EventLoopStart());

  ASSERT_TRUE(ewk_view_scroll_set(GetEwkWebView(), 10, 10));
  ASSERT_TRUE(ewk_view_scroll_pos_get(GetEwkWebView(), &x, &y));
  ASSERT_EQ(10, x);
  ASSERT_EQ(10, y);

  ASSERT_TRUE(ewk_view_scroll_set(GetEwkWebView(), 20, 20));
  ASSERT_TRUE(ewk_view_scroll_pos_get(GetEwkWebView(), &x, &y));
  ASSERT_EQ(20, x);
  ASSERT_EQ(20, y);
}

/**
 * @brief ewk_view_scroll_pos_get() should return proper value after the engine tries to scroll over the page size
 */
TEST_F(utc_blink_ewk_view_scroll_pos_get, POS_TEST4)
{
  int x = 0, y = 0;

  ASSERT_TRUE(ewk_view_url_set(GetEwkWebView(),GetResourceUrl(resource).c_str()));
  ASSERT_EQ(Success, EventLoopStart());

  int scrollSizeX = -1, scrollSizeY = -1;
  ASSERT_TRUE(ewk_view_scroll_size_get(GetEwkWebView(), &scrollSizeX, &scrollSizeY));

  int oversizeX = scrollSizeX + 100;
  ASSERT_TRUE(ewk_view_scroll_set(GetEwkWebView(), oversizeX, 10));
  ASSERT_TRUE(ewk_view_scroll_pos_get(GetEwkWebView(), &x, &y));
  ASSERT_EQ(scrollSizeX, x);
  ASSERT_EQ(10, y);

  ecore_timer_add(5.0f, quitMainLoop, this);
  ASSERT_EQ(Success, EventLoopStart());

  ASSERT_TRUE(ewk_view_scroll_pos_get(GetEwkWebView(), &x, &y));
  ASSERT_NE(scrollSizeX, x);
  ASSERT_EQ(10, y);

  // Now at the end of the page
  ASSERT_TRUE(ewk_view_scroll_set(GetEwkWebView(), oversizeX, 10));
  ecore_timer_add(5.0f, quitMainLoop, this);
  ASSERT_EQ(Success, EventLoopStart());
  ASSERT_TRUE(ewk_view_scroll_pos_get(GetEwkWebView(), &x, &y));
  ASSERT_EQ(scrollSizeX, x);
}

/**
 * @brief Same as POS_TEST4, but with negative value
 */
TEST_F(utc_blink_ewk_view_scroll_pos_get, POS_TEST5)
{
  int x = 0, y = 0;

  ASSERT_TRUE(ewk_view_url_set(GetEwkWebView(),GetResourceUrl(resource).c_str()));
  ASSERT_EQ(Success, EventLoopStart());

  ASSERT_TRUE(ewk_view_scroll_set(GetEwkWebView(), -100, 10));
  ASSERT_TRUE(ewk_view_scroll_pos_get(GetEwkWebView(), &x, &y));
  ASSERT_EQ(0, x);
  ASSERT_EQ(10, y);

  ecore_timer_add(5.0f, quitMainLoop, this);
  ASSERT_EQ(Success, EventLoopStart());

  ASSERT_TRUE(ewk_view_scroll_pos_get(GetEwkWebView(), &x, &y));
  ASSERT_EQ(0, x);
  ASSERT_EQ(10, y);
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
