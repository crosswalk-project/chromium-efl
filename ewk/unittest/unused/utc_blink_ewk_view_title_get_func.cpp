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
#define TESTED_FUN_NAME ewk_view_title_get
#define POSITIVE_TEST_FUN_NUM 1
#define NEGATIVE_TEST_FUN_NUM 1

#include "utc_blink_ewk.h"

#ifndef NULL
#define NULL    0x0
#endif

#define SAMPLE_HTML_FILE       ("/common/sample.html")
#define TITLE0 "Test file"

extern struct Ewk_Test_View test_view;

class utc_blink_ewk_view_title_get : public ::testing::Test
{
 protected:
  utc_blink_ewk_view_title_get()
  {
  }

  ~utc_blink_ewk_view_title_get()
  {
  }

  /*Callback for load finished */
  static void loadFinished(void *data, Evas_Object *webview, void *arg)
  {
    utc_message("[loadFinished] :: \n");
    utc_blink_main_loop_quit();
  }

  /* Callback for load error */
  static void loadError(void* data, Evas_Object* webview, void* event_info)
  {
    utc_message("[loadError] :: \n");
    utc_blink_main_loop_quit();

    utc_fail();
  }

  /* Startup and cleanup functions */
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
    utc_blink_ewk_test_end();
  }

  /*Load Specified URL*/
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
 * @brief Checks title when page is loaded.
 */
TEST_F(utc_blink_ewk_view_title_get, POS_TEST)
{
  loadURL(SAMPLE_HTML_FILE);
  utc_blink_main_loop_begin();

  const char* title0 = ewk_view_title_get(test_view.webview);
  utc_check_str_eq(title0, TITLE0);
}

/**
 * @brief Checking title when ewk_view is NULL.
 */
TEST_F(utc_blink_ewk_view_title_get, NEG_TEST)
{
  const char* title = ewk_view_title_get(NULL);
  if (title)
    utc_fail();
}
