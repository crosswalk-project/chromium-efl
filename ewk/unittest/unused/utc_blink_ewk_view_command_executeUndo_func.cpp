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

#ifndef NULL
#define NULL    0x0
#endif

#define SAMPLE_HTML_FILE                ("/selection/sample_1.html")

static int result = 0;
extern struct Ewk_Test_View test_view;

class utc_blink_ewk_view_command_executeUndo : public ::testing::Test {
 protected:
  utc_blink_ewk_view_command_executeUndo() {
  }

  virtual ~utc_blink_ewk_view_command_executeUndo() {
  }

  virtual void SetUp()
  {
    utc_blink_ewk_test_init();
    evas_object_smart_callback_add(test_view.webview, "load,finished", loadFinished, NULL);
    evas_object_smart_callback_add(test_view.webview, "load,error", loadError, NULL);
    evas_object_smart_callback_add(test_view.webview,"undo,size", get_undo_size, 0);
  }

  virtual void TearDown()
  {
    evas_object_smart_callback_del(test_view.webview, "load,finished", loadFinished);
    evas_object_smart_callback_del(test_view.webview, "load,error", loadError);
    evas_object_smart_callback_del(test_view.webview,"undo,size", get_undo_size);
    utc_blink_ewk_test_end();
  }

  /* Load specified url */
  static void loadURL(const char* url)
  {
    utc_message("[loadURL] :: \n");

    char* full_path = generate_full_resource_path(url);
    if (!full_path)
      utc_fail();

    Eina_Bool result = ewk_view_url_set(test_view.webview, full_path);
    free(full_path);

    if (!result)
      utc_fail();
  }

  /* Callback for load finished */
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

  static void get_undo_size(void* data, Evas_Object* webview, void* event_info)
  {
    utc_message("[get_undo_size callback] :: \n");
    result = *(int*)event_info;
    utc_blink_main_loop_quit();
  }
};

/**
 * @brief Positive test case of ewk_view_command_executeUndo(),testing results to get undo stack size
 */
TEST_F(utc_blink_ewk_view_command_executeUndo, POS_TEST)
{
  loadURL(SAMPLE_HTML_FILE);
  utc_blink_main_loop_begin();
  utc_blink_main_loop_begin();

  ewk_view_command_execute(test_view.webview, "Copy", NULL);
  ewk_view_command_execute(test_view.webview, "Paste", NULL);
  utc_blink_main_loop_begin();
  ewk_view_command_execute(test_view.webview, "Undo", NULL);
  utc_blink_main_loop_begin();

  utc_check_eq(result, 1);
}
