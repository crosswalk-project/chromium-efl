/*
 * chromium EFL
 *
 * Copyright (c) 2012 Samsung Electronics Co., Ltd.
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

/* Define those macros _before_ you include the utc_webkit_ewk.h header file. */
#include "utc_blink_ewk.h"

#ifndef NULL
#define NULL    0x0
#endif

#define SAMPLE_HTML_FILE        ("/common/sample.html")

extern struct Ewk_Test_View test_view;
static int result = 0;
class utc_blink_ewk_view_didStartProvisionalLoadFrameCallback : public ::testing::Test
{
 protected:
  utc_blink_ewk_view_didStartProvisionalLoadFrameCallback()
  {
  }

  ~utc_blink_ewk_view_didStartProvisionalLoadFrameCallback()
  {
  }

  /* Callback for quitting main loop */
  static void provisional_load(void *data, Evas_Object *webview, void *arg)
  {
    utc_message("[provisional_load] :: \n");
    result = 1;
  }

  /* Startup function */
  void SetUp()
  {
    utc_blink_ewk_test_init();
    evas_object_smart_callback_add(test_view.webview, "load,provisional,started", provisional_load, NULL);
    evas_object_smart_callback_add(test_view.webview, "load,finished", loadFinished, NULL);
  }

  /* Cleanup function */
  void TearDown()
  {
    evas_object_smart_callback_del(test_view.webview, "load,provisional,started", provisional_load);
    evas_object_smart_callback_del(test_view.webview, "load,finished", loadFinished);
    utc_blink_ewk_test_end();
  }

  /* Callback for load finished */
  static void loadFinished(void* data, Evas_Object* webview, void* event_info)
  {
    utc_message("[loadFinished] :: \n");
    utc_blink_main_loop_quit();
  }

  /*Load specified URL */
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
};

/**
 * @brief Positive test case of ewk_view_resume()
 */
TEST_F(utc_blink_ewk_view_didStartProvisionalLoadFrameCallback, POS_TEST)
{
  loadURL(SAMPLE_HTML_FILE);
  utc_blink_main_loop_begin();

  if (result)
    utc_pass();
  else
    utc_fail();
}

/**
 * @brief Negative test case of ewk_view_resume()
 */
TEST_F(utc_blink_ewk_view_didStartProvisionalLoadFrameCallback, NEG_TEST)
{
  utc_pass();
}
