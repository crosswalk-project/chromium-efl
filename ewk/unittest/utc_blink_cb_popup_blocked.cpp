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
