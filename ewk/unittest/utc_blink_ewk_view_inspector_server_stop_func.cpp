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

class utc_blink_ewk_view_inspector_server_stop : public utc_blink_ewk_base
{
  void LoadFinished(Evas_Object* webview)
  {
    EventLoopStop(Success);
  }
};

/* @brief Positive test case of ewk_view_inspector_server_stop */
TEST_F(utc_blink_ewk_view_inspector_server_stop, StartStop)
{
  ASSERT_EQ(11111, ewk_view_inspector_server_start(GetEwkWebView(), 11111));
  EventLoopWait(1);

  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), "http://localhost:11111"));
  ASSERT_EQ(Success, EventLoopStart());

  ASSERT_EQ(EINA_TRUE, ewk_view_inspector_server_stop(GetEwkWebView()));
  EventLoopWait(1);

  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), "http://localhost:11111"));
  ASSERT_EQ(Failure, EventLoopStart());

  /* Checking if server can be started, to make sure that was stopped successfully. */
  ASSERT_NE(0, ewk_view_inspector_server_start(GetEwkWebView(), 0));
  EventLoopWait(1);
  EXPECT_EQ(EINA_TRUE, ewk_view_inspector_server_stop(GetEwkWebView()));
}

/* @brief Negative test case of ewk_view_inspector_server_stop */
TEST_F(utc_blink_ewk_view_inspector_server_stop, StopWithoutStart)
{
  EXPECT_NE(EINA_TRUE, ewk_view_inspector_server_stop(GetEwkWebView()));
}