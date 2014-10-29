// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

#define URL  ("http://google.com")

class utc_blink_cb_popup_blocked : public utc_blink_ewk_base
{
protected:
  utc_blink_cb_popup_blocked()
    : target_uri(NULL)
  {
  }

  void PostSetUp()
  {
    evas_object_smart_callback_add(GetEwkWebView(), "popup,blocked", ToSmartCallback(popup_blocked_cb), this);
  }

  void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(), "popup,blocked", ToSmartCallback(popup_blocked_cb));
  }

  static void popup_blocked_cb(utc_blink_cb_popup_blocked* owner, Evas_Object*, Eina_Stringshare* uri)
  {
    ASSERT_TRUE(owner);
    owner->target_uri = eina_stringshare_add(uri);
    owner->EventLoopStop(Success);
  }

  static void window_open_script_executed(Evas_Object* o, const char* result_value, utc_blink_cb_popup_blocked* owner)
  {
    ASSERT_TRUE(owner);
    owner->EventLoopStop(Failure);
  }

protected:
  Eina_Stringshare* target_uri;
};

/**
 * @brief Tests "popup,blocked" callback
 *
 * By default chromium-efl will block all window.open JS requests that were not produced by "user gesture"
 */
TEST_F(utc_blink_cb_popup_blocked, callback)
{
  char htmlBuffer[] = "<html>"
                        "<head></head>"
                        "<body onload=\"window.open('http://www.google.com/')\"></body>"
                      "</html>";

  ASSERT_EQ(EINA_TRUE, ewk_view_html_string_load(GetEwkWebView(), htmlBuffer, NULL, NULL));
  ASSERT_EQ(Success, EventLoopStart());
  ASSERT_STREQ("http://www.google.com/", target_uri);

  // ewk_view_script_execute works like "user gesture" so window.open should not be blocked
  ASSERT_EQ(EINA_TRUE, ewk_view_script_execute(GetEwkWebView(), "window.open('http://www.google.com/');", reinterpret_cast<Ewk_View_Script_Execute_Cb>(window_open_script_executed), this));
  ASSERT_EQ(Failure, EventLoopStart());
}
