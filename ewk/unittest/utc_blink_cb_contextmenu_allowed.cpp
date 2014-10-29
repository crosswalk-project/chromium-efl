// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_cb_contextmenu_allowed: public utc_blink_ewk_base
{
protected:
  utc_blink_cb_contextmenu_allowed()
    : allow(EINA_FALSE)
  {
  }

  void PostSetUp()
  {
    evas_object_smart_callback_add(GetEwkWebView(), "contextmenu,allowed", ToSmartCallback(contextmenu_allowed_cb), this);
  }

  void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(), "contextmenu,allowed", ToSmartCallback(contextmenu_allowed_cb));
  }

  static void contextmenu_allowed_cb(utc_blink_cb_contextmenu_allowed* owner, Evas_Object* webview, Eina_Bool* allowed)
  {
    ASSERT_TRUE(owner);

    if (allowed) {
      owner->allow = *allowed;
    }

    // callback has come, so we stop with success
    owner->EventLoopStop(Success);
  }

protected:
  Eina_Bool allow;
};

TEST_F(utc_blink_cb_contextmenu_allowed, callback)
{
  // selecting text inside text area should invoke context menu

  const char htmlBuff[] = "<html>"
                          "<body onload=\"document.getElementById('dd').select()\">"
                          "<textarea id=\"dd\">Test fullscreen</textarea>"
                          "</body>"
                          "</html>";

  ASSERT_EQ(EINA_TRUE, ewk_view_html_string_load(GetEwkWebView(), htmlBuff, NULL, NULL));

  // Wait for context menu callback
  ASSERT_EQ(Success, EventLoopStart());
  // It's hard to calculate position of context menu that will appear so we check only if value was modified
  ASSERT_EQ(EINA_TRUE, allow);
}
