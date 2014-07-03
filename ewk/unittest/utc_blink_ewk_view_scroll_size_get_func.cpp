// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_view_scroll_size_get : public utc_blink_ewk_base
{
 protected:
  void PostSetUp()
  {
    evas_object_smart_callback_add(GetEwkWebView(), "frame,rendered", frameRendered, this);
  }

  void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(), "frame,rendered", frameRendered);
  }

  static void frameRendered(void* data, Evas_Object* eObject, void* dataFinished)
  {
    utc_message("[frameRendered] :: \n");
    if(data)
      static_cast<utc_blink_ewk_view_scroll_size_get*>(data)->EventLoopStop(Success);
  }

protected:
  static const char*const resource;
};

const char*const utc_blink_ewk_view_scroll_size_get::resource="/ewk_view/index_big_red_square.html";

/**
 * @brief Positive test case of ewk_view_scroll_size_get(), API should return with true when passing correct arguments.
 */
TEST_F(utc_blink_ewk_view_scroll_size_get, POS_TEST)
{
  ASSERT_TRUE(ewk_view_url_set(GetEwkWebView(),GetResourceUrl(resource).c_str()));
  ASSERT_EQ(Success, EventLoopStart());

  int xPossibleScrollArea = 0, yPossibleScrollArea = 0;
  ASSERT_TRUE(ewk_view_scroll_size_get(GetEwkWebView(), &xPossibleScrollArea, &yPossibleScrollArea));
  ASSERT_GT(xPossibleScrollArea, 0);
  ASSERT_GT(yPossibleScrollArea, 0);
}

/**
  * @brief Negative test case of ewk_view_scroll_size_get(), testing for null.
  */
TEST_F(utc_blink_ewk_view_scroll_size_get, NEG_TEST)
{
  ASSERT_TRUE(ewk_view_url_set(GetEwkWebView(),GetResourceUrl(resource).c_str()));
  ASSERT_EQ(Success, EventLoopStart());

  int xPossibleScrollArea = 0, yPossibleScrollArea = 0;
  ASSERT_TRUE(ewk_view_scroll_size_get(GetEwkWebView(), NULL, &yPossibleScrollArea));
  ASSERT_GT(yPossibleScrollArea, 0);

  ASSERT_TRUE(ewk_view_scroll_size_get(GetEwkWebView(), &xPossibleScrollArea, NULL));
  ASSERT_GT(xPossibleScrollArea, 0);

  ASSERT_TRUE(ewk_view_scroll_size_get(GetEwkWebView(), NULL, NULL));

  ASSERT_FALSE(ewk_view_scroll_size_get(NULL, &xPossibleScrollArea, &yPossibleScrollArea));
  ASSERT_EQ(0, xPossibleScrollArea);
  ASSERT_EQ(0, yPossibleScrollArea);
}
