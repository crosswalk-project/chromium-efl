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
#include "utc_blink_ewk.h"

#define SAMPLE_HTML_FILE                ("/common/sample.html")

extern struct Ewk_Test_View test_view;

int load_start_result = 0;
int load_finished_result = 0;
int load_progress_started = 0;
int load_progress_finished = 0;
int load_progress = 0;

class utc_blink_ewk_view_load_callbacks : public ::testing::Test {
 protected:
  utc_blink_ewk_view_load_callbacks() {
  }

  virtual ~utc_blink_ewk_view_load_callbacks() {
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
    load_finished_result = 1;
    utc_blink_main_loop_quit();
  }

  static void loadStarted(void* data, Evas_Object* webview, void* event_info)
  {
    utc_message("[loadStarted] :: \n");
    load_start_result = 1;
  }

  static void loadProgress(void* data, Evas_Object* webview, void* event_info)
  {
    utc_message("[loadProgress] :: \n");
    load_progress = 1;
  }
  static void loadProgressStarted(void* data, Evas_Object* webview, void* event_info)
  {
    utc_message("[loadProgressStarted] :: \n");
    load_progress_started = 1;
  }

  static void loadProgressFinished(void* data, Evas_Object* webview, void* event_info)
  {
    utc_message("[loadProgressFinished] :: \n");
    load_progress_finished = 1;
  }

  /* Callback for load error */
  static void loadError(void* data, Evas_Object* webview, void* event_info)
  {
    utc_message("[load_error] :: \n");
    utc_blink_main_loop_quit();

    utc_fail();
  }

  /* Startup function */
  virtual void SetUp()
  {
    utc_blink_ewk_test_init();
    evas_object_smart_callback_add(test_view.webview, "load,finished", loadFinished, NULL);
    evas_object_smart_callback_add(test_view.webview, "load,finished", loadFinished, NULL);
    evas_object_smart_callback_add(test_view.webview, "load,started", loadStarted, NULL);
    evas_object_smart_callback_add(test_view.webview, "load,progress", loadProgress, NULL);
    evas_object_smart_callback_add(test_view.webview, "load,progress,started", loadProgressStarted, NULL);
    evas_object_smart_callback_add(test_view.webview, "load,progress,finished", loadProgressFinished, NULL);
    evas_object_smart_callback_add(test_view.webview, "load,error", loadError, NULL);
  }

  /* Cleanup function */
  virtual void TearDown()
  {
    evas_object_smart_callback_del(test_view.webview, "load,finished", loadFinished);
    evas_object_smart_callback_del(test_view.webview, "load,started", loadStarted);
    evas_object_smart_callback_del(test_view.webview, "load,progress", loadProgress);
    evas_object_smart_callback_del(test_view.webview, "load,progress,started", loadProgressStarted);
    evas_object_smart_callback_del(test_view.webview, "load,progress,finished", loadProgressFinished);
    evas_object_smart_callback_del(test_view.webview, "load,error", loadError);
    utc_blink_ewk_test_end();
  }
};

/**
* @brief Check whether load,finished is called
*/
TEST_F(utc_blink_ewk_view_load_callbacks, POS_TEST1)
{
  loadURL(SAMPLE_HTML_FILE);
  utc_blink_main_loop_begin();
  utc_check_eq(load_finished_result, 1);
}

/**
* @brief Change whether load,started is called
*/
TEST_F(utc_blink_ewk_view_load_callbacks, POS_TEST2)
{
  loadURL(SAMPLE_HTML_FILE);
  utc_blink_main_loop_begin();
  utc_check_eq(load_start_result, 1);
}

/**
* @brief Change whether load,progress is called
*/
TEST_F(utc_blink_ewk_view_load_callbacks, POS_TEST3)
{
  loadURL(SAMPLE_HTML_FILE);
  utc_blink_main_loop_begin();
  utc_check_eq(load_progress, 1);
}

/**
* @brief Checking whether load,progress,started is called
*/
TEST_F(utc_blink_ewk_view_load_callbacks, POS_TEST4)
{
  loadURL(SAMPLE_HTML_FILE);
  utc_blink_main_loop_begin();
  utc_check_eq(load_progress_started, 1);
}

/**
* @brief Checking whether load,progress,finished is called
*/
TEST_F(utc_blink_ewk_view_load_callbacks, POS_TEST5)
{
  loadURL(SAMPLE_HTML_FILE);
  utc_blink_main_loop_begin();
  utc_check_eq(load_progress_finished, 1);
}