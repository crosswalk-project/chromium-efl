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

#define SAMPLE_HTML_FILE_EDGE_BOTTOM       ("/ewk_view/edge_test_bottom.html")
#define SAMPLE_HTML_FILE_EDGE_TOP          ("/ewk_view/edge_test_top.html")
#define SAMPLE_HTML_FILE_EDGE_LEFT         ("/ewk_view/edge_test_left.html")
#define SAMPLE_HTML_FILE_EDGE_RIGHT        ("/ewk_view/edge_test_right.html")

static Eina_Bool did_edge_bottom;
static Eina_Bool did_edge_right;
static Eina_Bool did_edge_left;
static Eina_Bool did_edge_top;

extern struct Ewk_Test_View test_view;

class utc_blink_ewk_edge_notifications : public ::testing::Test {
  protected:
    utc_blink_ewk_edge_notifications() {
      did_edge_bottom = EINA_FALSE;
      did_edge_top = EINA_FALSE;
      did_edge_left = EINA_FALSE;
      did_edge_right = EINA_FALSE;
    }

    virtual ~utc_blink_ewk_edge_notifications() {
    }

    virtual void SetUp() {
      utc_blink_ewk_test_init();
      evas_object_smart_callback_add(test_view.webview, "load,finished", load_finished, NULL);
      evas_object_smart_callback_add(test_view.webview, "load,error", load_error, NULL);
      evas_object_smart_callback_add(test_view.webview, "edge,bottom", edge_bottom, NULL);
      evas_object_smart_callback_add(test_view.webview, "edge,top", edge_top, NULL);
      evas_object_smart_callback_add(test_view.webview, "edge,right", edge_right, NULL);
      evas_object_smart_callback_add(test_view.webview, "edge,left", edge_left, NULL);
    }

    virtual void TearDown() {
      evas_object_smart_callback_del(test_view.webview, "edge,bottom", edge_bottom);
      evas_object_smart_callback_del(test_view.webview, "edge,top", edge_top);
      evas_object_smart_callback_del(test_view.webview, "edge,right", edge_right);
      evas_object_smart_callback_del(test_view.webview, "edge,left", edge_left);
      evas_object_smart_callback_del(test_view.webview, "load,finished", load_finished);
      evas_object_smart_callback_del(test_view.webview, "load,error", load_error);
      utc_blink_ewk_test_end();
    }

    static void load_finished(void* data, Evas_Object* webview, void* event_info)
    {
      utc_message("[load_finished] :: \n");
    }

    static void load_error(void* data, Evas_Object* webview, void* event_info)
    {
      utc_message("[load_error] :: \n");
      utc_blink_main_loop_quit();
    }

    static void edge_bottom(void* data, Evas_Object* webview, void* event_info)
    {
      if(!did_edge_bottom) {
       did_edge_bottom = EINA_TRUE;
       utc_blink_main_loop_quit();
      }
    }

    static void edge_top(void* data, Evas_Object* webview, void* event_info)
    {
      did_edge_top = EINA_TRUE;
       utc_blink_main_loop_quit();
    }

    static void edge_left(void* data, Evas_Object* webview, void* event_info)
    {
      did_edge_left = EINA_TRUE;
      utc_blink_main_loop_quit();
    }

    static void edge_right(void* data, Evas_Object* webview, void* event_info)
    {
      did_edge_right = EINA_TRUE;
      utc_blink_main_loop_quit();
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
* @brief Tests if edge bottom notification was invoked
*/
TEST_F(utc_blink_ewk_edge_notifications, POS_TEST1)
{
  loadURL(SAMPLE_HTML_FILE_EDGE_BOTTOM);
  utc_blink_main_loop_begin();
  utc_check_eq(did_edge_bottom, EINA_TRUE);
}

/**
* @brief Tests if edge top notification was invoked
*/
TEST_F(utc_blink_ewk_edge_notifications, POS_TEST2)
{
  loadURL(SAMPLE_HTML_FILE_EDGE_TOP);
  utc_blink_main_loop_begin();
  utc_check_eq(did_edge_top, EINA_TRUE);
}

/**
* @brief Tests if edge left notification was invoked
*/
TEST_F(utc_blink_ewk_edge_notifications, POS_TEST3)
{
  loadURL(SAMPLE_HTML_FILE_EDGE_LEFT);
  utc_blink_main_loop_begin();
  utc_check_eq(did_edge_left, EINA_TRUE);
}

/**
* @brief Tests if edge right notification was invoked
*/
TEST_F(utc_blink_ewk_edge_notifications, POS_TEST4)
{
  loadURL(SAMPLE_HTML_FILE_EDGE_RIGHT);
  utc_blink_main_loop_begin();
  utc_check_eq(did_edge_right, EINA_TRUE);
}