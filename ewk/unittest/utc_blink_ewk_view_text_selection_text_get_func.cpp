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

class utc_blink_ewk_view_text_selection_text_get : public utc_blink_ewk_base {
protected:
  void PostSetUp()
  {
    evas_object_smart_callback_add(GetEwkWebView(), "textselection,mode", ToSmartCallback(textselection_mode_cb), this);
  }

  void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(), "textselection,mode", ToSmartCallback(textselection_mode_cb));
  }

  static void textselection_mode_cb(utc_blink_ewk_view_text_selection_text_get* owner, Evas_Object* webview, bool* data)
  {
    ASSERT_TRUE(owner);
    owner->EventLoopStop(Success);
  }

  static void load_finished_cb(utc_blink_ewk_view_text_selection_text_get* owner, Evas_Object* webview, bool* data)
  {
    ASSERT_TRUE(owner);
    owner->EventLoopStop(Success);
  }
};

/**
 * @brief Checks if API returns proper value if selection event occured from JavaScript
 */
TEST_F(utc_blink_ewk_view_text_selection_text_get, textarea_js_select)
{
  // In case of no error and no selection, ewk_view_text_selection_text_get should return EMPTY string, not NULL ptr
  const char* selected_text = ewk_view_text_selection_text_get(GetEwkWebView());
  ASSERT_TRUE(selected_text);
  ASSERT_STREQ("", selected_text);

  char htmlBuffer[] = "<!DOCTYPE html>"
                      "<html><head>"
                      "<script>"
                      "window.onload = function()"
                      "{ var text_area = document.getElementById('txtarea'); text_area.focus(); text_area.select();}"
                      "</script></head>"
                      "<body>"
                      "<textarea id='txtarea'>select text</textarea>"
                      "</body></html>";

  ASSERT_EQ(EINA_TRUE, ewk_view_html_string_load(GetEwkWebView(), htmlBuffer, NULL, NULL));
  ASSERT_EQ(Success, EventLoopStart());

  selected_text = ewk_view_text_selection_text_get(GetEwkWebView());
  ASSERT_TRUE(selected_text);
  ASSERT_STREQ("select text", selected_text);
  fprintf(stderr,"[text_selection_text_get]=%s\n",selected_text);
}

/**
 * @brief Checks if API returns proper value if selection event occured by executing edit command "SelectAll"
 */
TEST_F(utc_blink_ewk_view_text_selection_text_get, execute_select_all)
{
  evas_object_smart_callback_auto autocb(GetEwkWebView(), "load,finished", ToSmartCallback(load_finished_cb), this);

  // In case of no error and no selection, ewk_view_text_selection_text_get should return EMPTY string, not NULL ptr
  const char* selected_text = ewk_view_text_selection_text_get(GetEwkWebView());
  ASSERT_TRUE(selected_text);
  ASSERT_STREQ("", selected_text);

  char htmlBuffer[] = "<!DOCTYPE html>"
                      "<html>"
                      "<body>"
                      "Some text"
                      "</body></html>";

  ASSERT_EQ(EINA_TRUE, ewk_view_html_string_load(GetEwkWebView(), htmlBuffer, NULL, NULL));
  ASSERT_EQ(Success, EventLoopStart());

  selected_text = ewk_view_text_selection_text_get(GetEwkWebView());
  ASSERT_TRUE(selected_text);
  ASSERT_STREQ("", selected_text);

  ASSERT_EQ(EINA_TRUE, ewk_view_command_execute(GetEwkWebView(), "SelectAll", NULL));
  ASSERT_EQ(Success, EventLoopStart());

  selected_text = ewk_view_text_selection_text_get(GetEwkWebView());
  ASSERT_TRUE(selected_text);
  ASSERT_STREQ("Some text", selected_text);
  fprintf(stderr,"[text_selection_text_get]=%s\n",selected_text);
}

/**
 * @brief  checking whether function is working when providing NULL value.
 */
TEST_F(utc_blink_ewk_view_text_selection_text_get, invalid_args)
{
  EXPECT_EQ(0, ewk_view_text_selection_text_get(NULL));
}

