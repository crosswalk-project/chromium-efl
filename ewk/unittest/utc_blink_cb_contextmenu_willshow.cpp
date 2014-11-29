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
