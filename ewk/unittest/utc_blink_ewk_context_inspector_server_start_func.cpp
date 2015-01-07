// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_context_inspector_server_start : public utc_blink_ewk_base
{
  void LoadFinished(Evas_Object* webview)
  {
    EventLoopStop(Success);
  }
};

/* @brief Negative test case of ewk_context_inspector_server_start */
TEST_F(utc_blink_ewk_context_inspector_server_start, NullArg)
{
  ASSERT_NE(EINA_TRUE, ewk_context_inspector_server_start(NULL,0));
}

/* @brief Try start inspector of context on specified TCP port*/
TEST_F(utc_blink_ewk_context_inspector_server_start, Start)
{
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), "http://localhost:11111"));
  ASSERT_NE(Success, EventLoopStart()) << "http://localhost:11111";

  ASSERT_EQ(11111, ewk_context_inspector_server_start(ewk_context_default_get(), 11111));
  EventLoopWait(1);

  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), "http://localhost:11111"));
  ASSERT_EQ(Success, EventLoopStart()) << "http://localhost:11111";
}

/* @brief Try start inspector of context on unspecified TCP port*/
TEST_F(utc_blink_ewk_context_inspector_server_start, DefaultPort)
{
  int port=ewk_context_inspector_server_start(ewk_context_default_get(), 0);
  ASSERT_NE(0, port) << "ewk_context_inspector_server_start(0)";
  EventLoopWait(1);

  char url[32]; sprintf(url, "http://localhost:%u", port);
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), url));
  ASSERT_EQ(Success, EventLoopStart()) << url;

}

/* @brief Negative test case of ewk_context_inspector_server_start on used port*/
TEST_F(utc_blink_ewk_context_inspector_server_start, UsedPort)
{
  ASSERT_EQ(11111, ewk_view_inspector_server_start(GetEwkWebView(),11111));
  EventLoopWait(1);
  ASSERT_NE(11111, ewk_context_inspector_server_start(ewk_context_default_get(),11111));
}
