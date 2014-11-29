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

#define SAMPLE_HTML_FILE1                ("/common/sample.html")
#define SAMPLE_HTML_FILE2                ("/common/sample_1.html")
#define SAMPLE_HTML_FILE3                ("/common/sample_2.html")

extern struct Ewk_Test_View test_view;

class utc_blink_ewk_view_back_forward_list_clear : public ::testing::Test {
 protected:
  utc_blink_ewk_view_back_forward_list_clear() {
  }

  virtual ~utc_blink_ewk_view_back_forward_list_clear() {
  }

  virtual void SetUp() {
    utc_blink_ewk_test_init();
    evas_object_smart_callback_add(test_view.webview, "load,finished", loadFinished, NULL);
    evas_object_smart_callback_add(test_view.webview, "load,error", loadError, NULL);
  }

  virtual void TearDown() {
    evas_object_smart_callback_del(test_view.webview, "load,finished", loadFinished);
    evas_object_smart_callback_del(test_view.webview, "load,error", loadError);
    utc_blink_ewk_test_end();
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
    utc_message("[load_error] :: \n");
    utc_blink_main_loop_quit();

    utc_fail();
  }
};

/**
 * @brief checking if function works properly when no page is loaded and passing correct webview.
 */
TEST_F(utc_blink_ewk_view_back_forward_list_clear, POS_TEST1)
{
  ewk_view_back_forward_list_clear(test_view.webview);
  utc_pass();
}

/**
 * @brief checking if when two pages are loaded and function is called, then ewk_view_back_possible
 * should return EINA_FALSE
 */
TEST_F(utc_blink_ewk_view_back_forward_list_clear, POS_TEST2)
{
  loadURL(SAMPLE_HTML_FILE1);
  utc_blink_main_loop_begin();

  loadURL(SAMPLE_HTML_FILE2);
  utc_blink_main_loop_begin();

  ewk_view_back_forward_list_clear(test_view.webview);
  Eina_Bool result = ewk_view_back_possible(test_view.webview);

  utc_check_eq(result, EINA_FALSE);
}

/**
 * @brief checking if when two pages are loaded, ewk_view_back is called and then ewk_view_back_forward_list_clear
 * is called, then ewk_view_forward_possible should return EINA_FALSE
 */
TEST_F(utc_blink_ewk_view_back_forward_list_clear, POS_TEST3)
{
  loadURL(SAMPLE_HTML_FILE1);
  utc_blink_main_loop_begin();

  loadURL(SAMPLE_HTML_FILE2);
  utc_blink_main_loop_begin();

  ewk_view_back(test_view.webview);
  utc_blink_main_loop_begin();

  ewk_view_back_forward_list_clear(test_view.webview);
  Eina_Bool result = ewk_view_forward_possible(test_view.webview);

  utc_check_eq(result, EINA_FALSE);
}

/**
 * @brief checking if when three pages are loaded, ewk_view_back is called and then ewk_view_back_forward_list_clear
 * is called, then ewk_view_forward_possible and ewk_view_back_possible both should return EINA_FALSE
 */
TEST_F(utc_blink_ewk_view_back_forward_list_clear, POS_TEST4)
{
  loadURL(SAMPLE_HTML_FILE1);
  utc_blink_main_loop_begin();

  loadURL(SAMPLE_HTML_FILE2);
  utc_blink_main_loop_begin();

  loadURL(SAMPLE_HTML_FILE3);
  utc_blink_main_loop_begin();

  ewk_view_back(test_view.webview);
  utc_blink_main_loop_begin();

  ewk_view_back_forward_list_clear(test_view.webview);
  Eina_Bool result = !ewk_view_forward_possible(test_view.webview) && !ewk_view_back_possible(test_view.webview);

  utc_check_eq(result, EINA_TRUE);
}

/**
 * @brief checking if function works properly in case of NULL webview
 */
TEST_F(utc_blink_ewk_view_back_forward_list_clear, NEG_TEST)
{
  ewk_view_back_forward_list_clear(NULL);
  utc_pass();
}
