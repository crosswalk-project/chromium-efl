// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_view_main_frame_get : public utc_blink_ewk_base
{
protected:
  void LoadFinished(Evas_Object* webview) override
  {
    EventLoopStop(Success);
  }
};

TEST_F(utc_blink_ewk_view_main_frame_get, POS_TEST)
{
  const char simpleHTML[] = "<html><body>test</body></html>";

  ASSERT_EQ(EINA_TRUE, ewk_view_html_string_load(GetEwkWebView(), simpleHTML, nullptr, nullptr));
  ASSERT_EQ(Success, EventLoopStart());
  ASSERT_NE(nullptr, ewk_view_main_frame_get(GetEwkWebView()));
  ASSERT_EQ(EINA_TRUE, ewk_frame_is_main_frame(ewk_view_main_frame_get(GetEwkWebView())));
}

TEST_F(utc_blink_ewk_view_main_frame_get, INVALID_ARG)
{
  ASSERT_EQ(nullptr, ewk_view_main_frame_get(nullptr));
}
