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