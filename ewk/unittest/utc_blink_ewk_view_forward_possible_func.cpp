// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_view_forward_possible_func : public utc_blink_ewk_base
{
protected:
  /* Callback for load finished */
  void LoadFinished(Evas_Object* webview)
  {
    EventLoopStop(Success);
  }
};

/**
 * @brief Check whether it is possible to go forward when no webpage is loaded
 */
TEST_F(utc_blink_ewk_view_forward_possible_func, WithoutLoad)
{
  EXPECT_FALSE(ewk_view_forward_possible(GetEwkWebView()));
}

/**
 * @brief Change whether it is possible to go forward without go back
 */
TEST_F(utc_blink_ewk_view_forward_possible_func, WithoutBack)
{
  ASSERT_TRUE(ewk_view_url_set(GetEwkWebView(), GetResourceUrl("common/sample.html").c_str()));
  ASSERT_EQ(Success, EventLoopStart());
  EXPECT_FALSE(ewk_view_forward_possible(GetEwkWebView()));
}

/**
 * @brief Change whether it is possible to go forward after go back
 */
TEST_F(utc_blink_ewk_view_forward_possible_func, AfterBack)
{
  ASSERT_TRUE(ewk_view_url_set(GetEwkWebView(), GetResourceUrl("common/sample.html").c_str()));
  ASSERT_EQ(Success, EventLoopStart());

  ASSERT_TRUE(ewk_view_url_set(GetEwkWebView(), GetResourceUrl("common/sample_1.html").c_str()));
  ASSERT_EQ(Success, EventLoopStart());

  ASSERT_TRUE(ewk_view_back(GetEwkWebView()));
  ASSERT_EQ(Success, EventLoopStart());

  EXPECT_TRUE(ewk_view_forward_possible(GetEwkWebView()));
}

/**
 * @brief Checking return value in case invalid argument.
 */
TEST_F(utc_blink_ewk_view_forward_possible_func, InvalidArg)
{
  EXPECT_FALSE(ewk_view_forward_possible(NULL));
}
