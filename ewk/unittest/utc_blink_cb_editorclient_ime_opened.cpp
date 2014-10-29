// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_cb_editorclient_ime_opened : public utc_blink_ewk_base
{
protected:
  void PreSetUp()
  {
    const char* default_context_id = ecore_imf_context_default_id_get();
    ASSERT_TRUE(default_context_id) << "This TC can't be run without default imf context - it will fail anyway";
  }

  void PostSetUp()
  {
    evas_object_smart_callback_add(GetEwkWebView(), "editorclient,ime,opened", ToSmartCallback(editorclient_ime_opened_cb), this);
  }

  void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(), "editorclient,ime,opened", ToSmartCallback(editorclient_ime_opened_cb));
  }

  void LoadFinished(Evas_Object*)
  {
    EventLoopStop(utc_blink_ewk_base::Success);
  }

  static void editorclient_ime_opened_cb(utc_blink_cb_editorclient_ime_opened* owner, Evas_Object*, void*)
  {
    ASSERT_TRUE(owner);
    owner->EventLoopStop(utc_blink_ewk_base::Success);
  }
};

/**
 * @brief Test "editorclient,ime,opened" callback
 */
TEST_F(utc_blink_cb_editorclient_ime_opened, callback)
{
  char htmlBuffer[] = "<html>"
                        "<head></head>"
                        "<body>"
                          "<input id=\"e\" type=\"text\"><a id=\"b\" onclick=\"document.getElementById('e').focus(); console.log('asdf');\">Button</a>"
                        "</body>"
                      "</html>";


  ASSERT_EQ(EINA_TRUE, ewk_view_html_string_load(GetEwkWebView(), htmlBuffer, NULL, NULL));
  // This TC will timeout on desktop as there is no input method context
  ASSERT_EQ(utc_blink_ewk_base::Success, EventLoopStart());

  // We need to focus on window
  elm_object_focus_set(GetEwkWebView(), EINA_TRUE);

  // This script execute should produce user gesture and should show software keyboard on mobile
  ASSERT_EQ(EINA_TRUE, ewk_view_script_execute(GetEwkWebView(), "document.getElementById('b').click();", NULL, NULL));
  // Wait for keyboard
  ASSERT_EQ(utc_blink_ewk_base::Success, EventLoopStart()) << "editorclient,ime,opened smart callback timeout";
}
