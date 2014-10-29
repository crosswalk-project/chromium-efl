// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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