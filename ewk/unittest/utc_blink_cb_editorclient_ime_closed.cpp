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

class utc_blink_cb_editorclient_ime_closed : public utc_blink_ewk_base
{
protected:
  void PreSetUp()
  {
    const char* default_context_id = ecore_imf_context_default_id_get();
    ASSERT_TRUE(default_context_id) << "This TC can't be run without default imf context - it will fail anyway";
  }

  void LoadFinished(Evas_Object*)
  {
    EventLoopStop(utc_blink_ewk_base::Success);
  }

  static void editorclient_ime_closed_cb(utc_blink_cb_editorclient_ime_closed* owner, Evas_Object*, void*)
  {
    ASSERT_TRUE(owner);
    owner->EventLoopStop(utc_blink_ewk_base::Success);
  }
};

/**
 * @brief Test "editorclient,ime,closed" callback
 */
TEST_F(utc_blink_cb_editorclient_ime_closed, callback)
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

  {
    evas_object_smart_callback_auto ime(GetEwkWebView(), "editorclient,ime,opened", ToSmartCallback(editorclient_ime_closed_cb), this);
    // This script execute should produce user gesture and should show software keyboard on mobile
    ASSERT_EQ(EINA_TRUE, ewk_view_script_execute(GetEwkWebView(), "document.getElementById('focus').click();", NULL, NULL));
    // Wait for keyboard
    ASSERT_EQ(utc_blink_ewk_base::Success, EventLoopStart()) << "editorclient,ime,opened smart callback timeout";
  }

  {
    evas_object_smart_callback_auto ime(GetEwkWebView(), "editorclient,ime,closed", ToSmartCallback(editorclient_ime_closed_cb), this);
    // This script execute should produce user gesture and should show software keyboard on mobile
    ASSERT_EQ(EINA_TRUE, ewk_view_script_execute(GetEwkWebView(), "document.getElementById('unfocus').click();", NULL, NULL));
    // Wait for keyboard
    ASSERT_EQ(utc_blink_ewk_base::Success, EventLoopStart()) << "editorclient,ime,opened smart callback timeout";
  }
}
