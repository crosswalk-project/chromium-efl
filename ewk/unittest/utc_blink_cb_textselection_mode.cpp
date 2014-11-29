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

class utc_blink_cb_textselection_mode : public utc_blink_ewk_base
{
protected:
  utc_blink_cb_textselection_mode()
    : state(Unknown)
  {
  }

  void PostSetUp()
  {
    ASSERT_EQ(Unknown, state);
    evas_object_smart_callback_add(GetEwkWebView(), "textselection,mode", ToSmartCallback(textselection_mode_cb), this);
  }

  void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(), "textselection,mode", ToSmartCallback(textselection_mode_cb));
  }

  static void textselection_mode_cb(utc_blink_cb_textselection_mode* owner, Evas_Object* webview, bool* data)
  {
    ASSERT_TRUE(owner);

    if (data) {
      owner->state = (*data ? Selected : Unselected);
      owner->EventLoopStop(Success);
    }

    EXPECT_TRUE(data);
    owner->EventLoopStop(Failure);
  }

protected:
  enum SelectionState
  {
    Unknown,
    Selected,
    Unselected
  };

  SelectionState state;
};

/**
 * @brief Tests "load,finished" callback with ewl_view_html_string_load
 */
TEST_F(utc_blink_cb_textselection_mode, selection_textarea)
{
  char htmlBuffer[] = "<!DOCTYPE html>"
                      "<html><head>"
                      "<script>"
                      "window.onload = function(){"
                      "var text_area = document.getElementById('txtarea');"
                      "text_area.focus();"
                      "text_area.select();}"
                      "</script></head>"
                      "<body>"
                      "<textarea id='txtarea'>select text</textarea>"
                      "</body></html>";

  ASSERT_EQ(EINA_TRUE, ewk_view_html_string_load(GetEwkWebView(), htmlBuffer, NULL, NULL));
  ASSERT_EQ(Success, EventLoopStart());
  ASSERT_EQ(Selected, state);
  ASSERT_EQ(EINA_TRUE, ewk_view_text_selection_clear(GetEwkWebView()));
  ASSERT_EQ(Success, EventLoopStart());
  ASSERT_EQ(Unselected, state);
}
