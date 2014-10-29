// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_cb_contextmenu_willshow: public utc_blink_ewk_base
{
protected:
  utc_blink_cb_contextmenu_willshow()
  {
    pos.x = -1;
    pos.y = -1;
  }

  void PostSetUp()
  {
    evas_object_smart_callback_add(GetEwkWebView(), "contextmenu,willshow", ToSmartCallback(contextmenu_willshow_cb), this);
  }

  void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(), "contextmenu,willshow", ToSmartCallback(contextmenu_willshow_cb));
  }

  static void contextmenu_willshow_cb(utc_blink_cb_contextmenu_willshow* owner, Evas_Object* webview, Evas_Point* position)
  {
    ASSERT_TRUE(owner);

    if (position) {
      owner->pos.x = position->x;
      owner->pos.y = position->y;
    }

    // callback has come, so we stop with success
    owner->EventLoopStop(Success);
  }

protected:
  Evas_Point pos;
};

TEST_F(utc_blink_cb_contextmenu_willshow, callback)
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
  ASSERT_NE(-1, pos.x);
  ASSERT_NE(-1, pos.y);
}
