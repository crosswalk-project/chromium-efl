// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/* Define those macros _before_ you include the utc_blink_ewk.h header file. */

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_text_style_position_get : public utc_blink_ewk_base {
 protected:
  static void get_text_style(void* data, Evas_Object* webview, void* event_info)
  {
    utc_message("[get_text_style callback] :: ");

    utc_blink_ewk_text_style_position_get* owner = NULL;

    if (!data) {
      utc_fail();
    }

    owner = static_cast<utc_blink_ewk_text_style_position_get*>(data);

    if (!event_info) {
      utc_fail();
    }

    Ewk_Text_Style* gStyle = (Ewk_Text_Style*)event_info;

    ewk_text_style_position_get(gStyle, &owner->leftpoint, &owner->rightpoint);
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
  Evas_Point leftpoint, rightpoint;
};


/**
 * @brief Positive test case of ewk_text_style_position_get(),testing results to get position.
 */
TEST_F(utc_blink_ewk_text_style_position_get, POS_TEST)
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

  utc_blink_ewk_base::MainLoopResult result = EventLoopStart(10.0);

  if (result != utc_blink_ewk_base::Success) {
    utc_fail();
  }

  utc_check_ne(leftpoint.x, rightpoint.x);
  utc_check_ne(leftpoint.y, rightpoint.y);
}

/**
 * @brief  checking whether function is working when providing NULL value.
 */
TEST_F(utc_blink_ewk_text_style_position_get, NEG_TEST)
{
  utc_check_eq( ewk_text_style_position_get(NULL, &leftpoint, &rightpoint), EINA_FALSE);
}

