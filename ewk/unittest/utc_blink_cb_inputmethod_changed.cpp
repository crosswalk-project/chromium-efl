// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_cb_inputmethod_changed : public utc_blink_ewk_base
{
protected:
  utc_blink_cb_inputmethod_changed()
  {
    memset(&panel_rect, 0, sizeof panel_rect);
  }
  void PreSetUp()
  {
    const char* default_context_id = ecore_imf_context_default_id_get();
    ASSERT_TRUE(default_context_id) << "This TC can't be run without default imf context - it will fail anyway";
  }

  void PostSetUp()
  {
    evas_object_smart_callback_add(GetEwkWebView(), "inputmethod,changed", ToSmartCallback(inputmethod_changed_cb), this);
  }

  void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(), "inputmethod,changed", ToSmartCallback(inputmethod_changed_cb));
  }

  void LoadFinished(Evas_Object*)
  {
    EventLoopStop(utc_blink_ewk_base::Success);
  }

  static void inputmethod_changed_cb(utc_blink_cb_inputmethod_changed* owner, Evas_Object*, Eina_Rectangle* rect)
  {
    utc_message("%p", rect);
    ASSERT_TRUE(owner);
    if  (rect) {
      owner->panel_rect = *rect;
    }

    owner->EventLoopStop(utc_blink_ewk_base::Success);
  }

protected:
  Eina_Rectangle panel_rect;
};

/**
 * @brief Test "inputmethod,changed" callback
 *
 * The name of the callback may be misguiding - it doesn't mean that the way you input text has changed.
 * It means that geometry of input panel has changed.
 */
TEST_F(utc_blink_cb_inputmethod_changed, callback)
{
  char htmlBuffer[] = "<html>"
                        "<head></head>"
                        "<body>"
                          "<input id=\"e\" type=\"text\"/><br/>"
                          "<a id=\"focus\" onclick=\"document.getElementById('e').focus();\">Focus</a><br/>"
                          "<a id=\"unfocus\" onclick=\"document.getElementById('e').blur();\">Unfocus</a><br/>"
                        "</body>"
                      "</html>";


  ASSERT_EQ(EINA_TRUE, ewk_view_html_string_load(GetEwkWebView(), htmlBuffer, NULL, NULL));
  // This TC will timeout on desktop as there is no input method context
  ASSERT_EQ(utc_blink_ewk_base::Success, EventLoopStart());

  // We need to focus on window
  elm_object_focus_set(GetEwkWebView(), EINA_TRUE);

  // This script execute should produce user gesture and should show software keyboard on mobile
  ASSERT_EQ(EINA_TRUE, ewk_view_script_execute(GetEwkWebView(), "document.getElementById('focus').click();", NULL, NULL));
  // Wait for keyboard

  // When opening ime geometry is sent two times (by efl). First rect will be empty, second not
  ASSERT_EQ(utc_blink_ewk_base::Success, EventLoopStart()) << "inputmethod,changed smart callback timeout";
  ASSERT_EQ(EINA_TRUE, eina_rectangle_is_empty(&panel_rect));

  ASSERT_EQ(utc_blink_ewk_base::Success, EventLoopStart()) << "inputmethod,changed smart callback timeout";
  ASSERT_EQ(EINA_FALSE, eina_rectangle_is_empty(&panel_rect));

  // This script execute should produce user gesture and should show software keyboard on mobile
  ASSERT_EQ(EINA_TRUE, ewk_view_script_execute(GetEwkWebView(), "document.getElementById('unfocus').click();", NULL, NULL));
  // Wait for keyboard
  ASSERT_EQ(utc_blink_ewk_base::Success, EventLoopStart()) << "inputmethod,changed smart callback timeout";
  ASSERT_EQ(EINA_TRUE, eina_rectangle_is_empty(&panel_rect));
}
