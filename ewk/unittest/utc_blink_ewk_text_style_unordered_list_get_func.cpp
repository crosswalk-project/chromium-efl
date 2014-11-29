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

/* Define those macros _before_ you include the utc_blink_ewk.h header file. */

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_text_style_unordered_list_get : public utc_blink_ewk_base {
protected:
  static void get_text_style(void* data, Evas_Object* webview, void* event_info)
  {
    utc_message("[get_text_style callback] :: ");

    utc_blink_ewk_text_style_unordered_list_get* owner = NULL;
    if (!data) {
      utc_fail();
    }

    owner = static_cast<utc_blink_ewk_text_style_unordered_list_get*>(data);

    Ewk_Text_Style* style = (Ewk_Text_Style*)event_info;
    owner->state = ewk_text_style_unordered_list_get(style);
    owner->EventLoopStop(utc_blink_ewk_base::Success);
  }

  virtual void PostSetUp()
  {
    evas_object_smart_callback_add(GetEwkWebView(),"text,style,state", get_text_style, this);

    Ewk_Settings* settings = ewk_view_settings_get(GetEwkWebView());
    ewk_settings_text_style_state_enabled_set(settings, EINA_TRUE);
  }

  virtual void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(),"text,style,state", get_text_style);
  }

protected:
  Ewk_Text_Style_State state;
};

/**
 * @brief Positive test case of ewk_text_style_bg_color_get(),testing results to get background Color.
 */
TEST_F(utc_blink_ewk_text_style_unordered_list_get, POS_TEST1)
{
  const char textStyleHTML[] =
   "<html>"
   "<head>"
   "<script>"
   "function selectText(element) {"
    "var doc = document;"
    "var text = doc.getElementById(element);"

    "if (doc.body.createTextRange) {"
        "var range = doc.body.createTextRange();"
        "range.moveToElementText(text);"
        "range.select();"
    "} else if (window.getSelection) {"
        "var selection = window.getSelection();"
        "var range = doc.createRange();"
        "range.selectNodeContents(text);"
        "selection.removeAllRanges();"
        "selection.addRange(range);"
    "}"
  "}"
   "function funcselect() {"
    "selectText('myText');"
  "}"

 "</script>"
 "<style>"
   "ul.a {list-style-type:circle;}"
   "ul.b {list-style-type:square;}"
   "ol.c {list-style-type:upper-roman;}"
   "ol.d {list-style-type:lower-alpha;}"
 "</style>"
   "</head>"
    "<body  onload='funcselect()'>"
    "<div id=\"myText\">"
      "<ul class=\"a\">"
        "<li>Coffee</li>"
        "<li>Tea</li>"
        "<li>Coca Cola</li>"
      "</ul>"
    "</div>"
    "</body>"
"</html>";
  ewk_view_html_string_load(GetEwkWebView(), textStyleHTML, 0, 0);

  utc_blink_ewk_base::MainLoopResult result = EventLoopStart(10.0);

  if (result != utc_blink_ewk_base::Success) {
    utc_fail();
  }

  utc_check_eq(state, EWK_TEXT_STYLE_STATE_TRUE);
}

TEST_F(utc_blink_ewk_text_style_unordered_list_get, POS_TEST2)
{
  const char textStyleHTML[] =
      "<!DOCTYPE html>"
      "<html><head>"
      "</head>"
      "<body  onload='txtarea.focus(); txtarea.select()'>"
      "<textarea id='txtarea' style='text-align:none;font-weight:bold;font-style:italic;background-color:rgb(0,255,0);"
      "font-size:20px;color:rgb(255,0,0)'>select text</textarea>"
      "</body></html>";
  ewk_view_html_string_load(GetEwkWebView(), textStyleHTML, 0, 0);

  utc_blink_ewk_base::MainLoopResult result = EventLoopStart(10.0);

  if (result != utc_blink_ewk_base::Success) {
    utc_fail();
  }

  utc_check_eq(state, EWK_TEXT_STYLE_STATE_FALSE);
}

/**
 * @brief  checking whether function is working when providing NULL value.
 */
TEST_F(utc_blink_ewk_text_style_unordered_list_get, NEG_TEST)
{
  utc_check_eq(EWK_TEXT_STYLE_STATE_FALSE, ewk_text_style_unordered_list_get(NULL));
}
