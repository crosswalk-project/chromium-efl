// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_view_user_agent_get : public utc_blink_ewk_base
{
protected:
  void LoadFinished(Evas_Object* webview)
  {
    EventLoopStop(utc_blink_ewk_base::Failure); // won't fail the test if EventLoopStop was already called
  }

  void PostSetUp()
  {
    evas_object_smart_callback_add(GetEwkWebView(), "title,changed", title_changed, this);
  }

  void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(), "title,changed", title_changed);
  }

  static void title_changed(void* data, Evas_Object* webview, void* event_info)
  {
    utc_message("[title changed] ::");
    if(data == NULL)
      FAIL();

    utc_blink_ewk_view_user_agent_get* owner = static_cast<utc_blink_ewk_view_user_agent_get*>(data);

    char* title = strdup(ewk_view_title_get(owner->GetEwkWebView()));
    char* userAgent =  strdup(ewk_view_user_agent_get(owner->GetEwkWebView()));

    if(strcmp(title, userAgent) == 0) {
      owner->EventLoopStop(utc_blink_ewk_base::Success);
    }
    free(title);
    free(userAgent);
    title = NULL;
    userAgent = NULL;
  }
};

/**
 * @brief Positive test case of ewk_view_user_agent_get()
 */
TEST_F(utc_blink_ewk_view_user_agent_get, POS_TEST)
{
   const char textStyleHTML[] =
    "<html>"
      "<head>"
        "<title>User Agent Test</title>"
        "<script>"
          "document.title=navigator.userAgent;"
        "</script>"
      "</head>"
      "<body></body>"
    "</html>";

  Eina_Bool result = ewk_view_html_string_load(GetEwkWebView(), textStyleHTML, 0, 0);
  if (!result)
    FAIL();

  utc_blink_ewk_base::MainLoopResult main_result = EventLoopStart();

  if (main_result != utc_blink_ewk_base::Success)
    FAIL();
}

/**
 * @brief Negative test case of ewk_view_user_agent_get()
 */
TEST_F(utc_blink_ewk_view_user_agent_get, NEG_TEST)
{
  const char* userAgent =  ewk_view_user_agent_get(NULL);
  if (NULL != userAgent)
    FAIL();
}