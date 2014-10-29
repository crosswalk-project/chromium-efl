// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

#define URL  ("http://google.com")

class utc_blink_cb_load_started : public utc_blink_ewk_base
{
protected:

  void LoadStarted(Evas_Object* webview)
  {
    EventLoopStop(utc_blink_ewk_base::Success);
  }

  void LoadFinished(Evas_Object* webview)
  {
    EventLoopStop(utc_blink_ewk_base::Failure);
  }
};

/**
 * @brief Tests "load,started" callback
 */
TEST_F(utc_blink_cb_load_started, callback)
{
    ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), URL));
    EXPECT_EQ(utc_blink_ewk_base::Success, EventLoopStart());
}
