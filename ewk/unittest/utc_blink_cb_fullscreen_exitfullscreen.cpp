// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_cb_fullscreen_exitfullscreen : public utc_blink_ewk_base
{
 protected:

  void PostSetUp()
  {
    evas_object_smart_callback_add(GetEwkWebView(), "fullscreen,exitfullscreen",
                                   ToSmartCallback(fullscreen_exitfullscreen), this);
    Ewk_Settings* settings = ewk_view_settings_get(GetEwkWebView());
    ASSERT_TRUE(NULL != settings);
    ewk_settings_fullscreen_enabled_set(settings,EINA_TRUE);
  }

  void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(), "fullscreen,exitfullscreen",
                                   ToSmartCallback(fullscreen_exitfullscreen));
  }

  void LoadFinished(Evas_Object*/* webview*/) {
    ewk_view_fullscreen_exit(GetEwkWebView());
  }

  static void fullscreen_exitfullscreen(utc_blink_cb_fullscreen_exitfullscreen* owner,
                                        Evas_Object*, void*)
  {
    utc_message("[ %s ] ::", __FUNCTION__);
    ASSERT_TRUE(NULL != owner);
    owner->EventLoopStop(Success);
  }
};

TEST_F(utc_blink_cb_fullscreen_exitfullscreen, POS_TEST)
{
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(),
                                        GetResourceUrl("ewk_view/fullscreen.html").c_str()));
  EXPECT_EQ(Success, EventLoopStart());
}
