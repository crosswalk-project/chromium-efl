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

class utc_blink_ewk_text_style_color_get : public utc_blink_ewk_base {
 protected:
  static void get_text_style(void* data, Evas_Object* webview, void* event_info)
  {
    utc_message("[get_text_style callback] :: ");

    utc_blink_ewk_text_style_color_get* owner = NULL;
    if (!data) {
      utc_fail();
    }

    owner = static_cast<utc_blink_ewk_text_style_color_get*>(data);

    Ewk_Text_Style* style = (Ewk_Text_Style*)event_info;

    Eina_Bool get_result = ewk_text_style_color_get(style, &owner->r, &owner->g, &owner->b, &owner->a);

    if (get_result != EINA_TRUE) {
      utc_fail();
    }

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
  int a, r, g, b;
};


/**
 * @brief Positive test case of ewk_text_style_color_get(),testing results to get Color.
 */
TEST_F(utc_blink_ewk_text_style_color_get, POS_TEST)
{
  const char textStyleHTML[] =
      "<!DOCTYPE html>"
      "<html><head>"
      "</head>"
      "<body  onload='txtarea.focus(); txtarea.select()'>"
      "<textarea id='txtarea' style='text-decoration:underline;font-weight:bold;font-style:italic;background-color:rgb(0,255,0);"
      "font-size:20px;color:rgb(255,0,0)'>select text</textarea>"
      "</body></html>";
  ewk_view_html_string_load(GetEwkWebView(), textStyleHTML, 0, 0);

  EventLoopStart(10.0);

  utc_check_eq(r, 255);
  utc_check_eq(g, 0);
  utc_check_eq(b, 0);
}

/**
 * @brief  checking whether function is working when providing NULL value.
 */
TEST_F(utc_blink_ewk_text_style_color_get, NEG_TEST)
{
  utc_check_eq(ewk_text_style_color_get(NULL, &a, &r, &g, &b), EINA_FALSE);
}
