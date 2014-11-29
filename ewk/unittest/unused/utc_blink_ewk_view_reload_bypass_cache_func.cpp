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
#define TESTED_FUN_NAME ewk_view_reload_bypass_cache
#define POSITIVE_TEST_FUN_NUM 1
#define NEGATIVE_TEST_FUN_NUM 1

#include "utc_blink_ewk.h"

#define SAMPLE_HTML_FILE        ("/common/sample.html")

extern struct Ewk_Test_View test_view;

class utc_blink_ewk_view_reload_bypass_cache : public ::testing::Test {
 protected:
  utc_blink_ewk_view_reload_bypass_cache() {
  }

  virtual ~utc_blink_ewk_view_reload_bypass_cache() {
  }

  static void loadFinished(void* data, Evas_Object* webview, void* event_info)
  {
    utc_message("[loadFinished] :: \n");
    utc_blink_main_loop_quit();
  }

  /* Callback for load error */
  static void loadError(void* data, Evas_Object* webview, void* event_info)
  {
    utc_message("[load_error] :: \n");
    utc_blink_main_loop_quit();

    utc_fail();
  }

  /* Startup and cleanup functions */
  virtual void SetUp()
  {
    utc_blink_ewk_test_init();
    evas_object_smart_callback_add(test_view.webview, "load,finished", loadFinished, NULL);
    evas_object_smart_callback_add(test_view.webview, "load,error", loadError, NULL);
  }

  virtual void TearDown()
  {
    evas_object_smart_callback_del(test_view.webview, "load,finished", loadFinished);
    evas_object_smart_callback_del(test_view.webview, "load,error", loadError);
    utc_blink_ewk_test_end();
  }

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
 * @brief Tests if returns TRUE when initiated with a correct webview.
 */
TEST_F(utc_blink_ewk_view_reload_bypass_cache, POS_TEST)
{
  loadURL(SAMPLE_HTML_FILE);
  utc_blink_main_loop_begin();

  Eina_Bool result = ewk_view_reload_bypass_cache(test_view.webview);
  utc_blink_main_loop_begin();

  utc_check_eq(result, EINA_TRUE);
}

/**
 * @brief Tests if returns FALSE when not initiated with NULL webview.
 */
TEST_F(utc_blink_ewk_view_reload_bypass_cache, NEG_TEST)
{
  Eina_Bool result = ewk_view_reload_bypass_cache(NULL);
  utc_check_eq(result, EINA_FALSE);
}
