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

#include "utc_blink_ewk.h"

extern struct Ewk_Test_View test_view;

Eina_Bool promptReply = EINA_FALSE;

#define SAMPLE_HTML_FILE        ("/ewk_view/prompt_test.html")

class utc_blink_ewk_view_javascript_prompt_reply : public ::testing::Test
{
 protected:
  utc_blink_ewk_view_javascript_prompt_reply()
  {
  }

  ~utc_blink_ewk_view_javascript_prompt_reply()
  {
  }

  void SetUp()
  {
    utc_blink_ewk_test_init();
    evas_object_smart_callback_add(test_view.webview, "load,finished", loadFinished, NULL);
    evas_object_smart_callback_add(test_view.webview, "load,error", loadError, NULL);
  }

  void TearDown()
  {
    evas_object_smart_callback_del(test_view.webview, "load,finished", loadFinished);
    evas_object_smart_callback_del(test_view.webview, "load,error", loadError);
    //utc_blink_ewk_test_end();
  }

  /* Load specified url */
  static void loadURL(const char* url)
  {
    char* full_path = generate_full_resource_path(url);
    if (!full_path)
      utc_fail();
    Eina_Bool result = ewk_view_url_set(test_view.webview, full_path);
      free(full_path);
    if (!result)
      utc_fail();
  }

  /* Callback for load finished */
  static void loadFinished(void* data, Evas_Object* webview, void* event_info)
  {
    utc_message("[loadFinished] :: \n");
    utc_blink_main_loop_quit();
  }

  /* Callback for load error */
  static void loadError(void* data, Evas_Object* webview, void* event_info)
  {
    utc_message("[loadError] :: \n");
    utc_blink_main_loop_quit();
  }

  /* Callback for script prompt */
  static Eina_Bool promptCallback(Evas_Object* webview ,const char* message,const char* default_value ,void* user_data)
  {
    utc_message("[promptCallback] :: xc\n");
    const char* result = "prompt_reply";
    ewk_view_javascript_prompt_reply(webview, result);
    promptReply = EINA_TRUE;
    utc_blink_main_loop_quit();
  }
};

/**
 * @brief Positive  test case of ewk_view_javascript_prompt_reply()
 * ewk_view_javascript_prompt_reply() should be performed without any problem.
 */
TEST_F(utc_blink_ewk_view_javascript_prompt_reply, POS_TEST)
{
  ewk_view_javascript_prompt_callback_set(test_view.webview, promptCallback, 0);
  loadURL(SAMPLE_HTML_FILE);
  utc_blink_main_loop_begin();
  utc_check_eq(promptReply, EINA_TRUE);
}

/**
* @brief Checking whether function works properly in case of NULL of a webview.
*/
TEST_F(utc_blink_ewk_view_javascript_prompt_reply, NEG_TEST)
{
  ewk_view_javascript_prompt_reply(NULL, NULL);
  utc_pass();
}
