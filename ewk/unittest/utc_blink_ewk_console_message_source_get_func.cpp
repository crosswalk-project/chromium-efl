// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

#define SAMPLE_HTML_FILE        ("/ewk_view/console_info.html")

class utc_blink_ewk_console_message_source_get : public utc_blink_ewk_base
{
 protected:

  std::string getSource;

  void PostSetUp()
  {
    evas_object_smart_callback_add(GetEwkWebView(),"console,message",console_message_cb, this);
  }

  void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(),"console,message",console_message_cb);
  }

  /* Callback for console message */
  static void console_message_cb(void* data, Evas_Object* webview, void* event_info)
  {
    utc_message("[console,message] :: \n");

    utc_blink_ewk_console_message_source_get* owner = static_cast<utc_blink_ewk_console_message_source_get*>(data);

    Ewk_Console_Message* console = (Ewk_Console_Message*)event_info;
    if(console) {

      owner->getSource = ewk_console_message_source_get(console);
      owner->EventLoopStop(utc_blink_ewk_base::Success);
    }
  }
};

/**
 * @brief Tests  ewk_console_message_source_get incase Ewk_Console_Message exists
 */
TEST_F(utc_blink_ewk_console_message_source_get, POS_TEST)
{
  getSource = '\0';
  Eina_Bool result = ewk_view_url_set(GetEwkWebView(), GetResourceUrl(SAMPLE_HTML_FILE).c_str());
  if (!result)
    FAIL();

  utc_blink_ewk_base::MainLoopResult main_result = EventLoopStart();
  if (main_result != utc_blink_ewk_base::Success)
    FAIL();

  utc_check_str_eq(getSource.c_str(), GetResourceUrl(SAMPLE_HTML_FILE).c_str());
}

/**
 * @brief Tests ewk_console_message_source_get incase Ewk_Console_Message does not exist
 */
TEST_F(utc_blink_ewk_console_message_source_get, NEG_TEST)
{
  utc_check_str_eq(NULL, ewk_console_message_source_get(NULL));
}
