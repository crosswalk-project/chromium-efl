// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_context_inspector_server_stop : public utc_blink_ewk_base
{
protected:
  void LoadFinished(Evas_Object* webview)
  {
    EventLoopStop(Success);
  }
};

/* @brief Try stop inspector with NULL as context */
TEST_F(utc_blink_ewk_context_inspector_server_stop, NullArg)
{
  ASSERT_NE(EINA_TRUE, ewk_context_inspector_server_stop(NULL));
}

/* @brief Try stop inspector without start */
TEST_F(utc_blink_ewk_context_inspector_server_stop, WithOutStart)
{
  ASSERT_NE(EINA_TRUE, ewk_context_inspector_server_stop(ewk_context_default_get()));
}

/* @brief Try start inspector after stop */
TEST_F(utc_blink_ewk_context_inspector_server_stop, StartStop)
{
  ASSERT_EQ(11111, ewk_context_inspector_server_start(ewk_context_default_get(), 11111));
  EventLoopWait(3);

  ASSERT_EQ(EINA_TRUE, ewk_context_inspector_server_stop(ewk_context_default_get()));
  EventLoopWait(3);

  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), "http://localhost:11111"));
  EXPECT_NE(Success, EventLoopStart());

  ASSERT_EQ(11111, ewk_context_inspector_server_start(ewk_context_default_get(), 11111));
  EventLoopWait(3);

  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), "http://localhost:11111"));
  EXPECT_EQ(Success, EventLoopStart());
}
