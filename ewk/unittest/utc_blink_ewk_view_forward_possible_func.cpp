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
