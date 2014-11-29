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

#define SAMPLE_HTML_FILE_UP_DOWN        ("/ewk_view/scroll_test_up_down.html")
#define SAMPLE_HTML_FILE_LEFT_RIGHT        ("/ewk_view/scroll_test_right_left.html")

static Eina_Bool is_failed;
static Eina_Bool is_Authenticated;
static Eina_Bool did_scroll_down;
static Eina_Bool did_scroll_right;
static Eina_Bool did_scroll_left;
static Eina_Bool did_scroll_up;

static const char* scrollDownScript = "window.scrollBy(0,50);";
// "var i=30; next(); function next(){ while(i){window.scrollTo(0,bodyDiv.scrollHeight); i-=10; setTimeout(next, 1000);} }";
static const char* scrollUpScript = "window.scrollBy(0,150); setTimeout('next()',1000); function next(){ window.scrollBy(0,-50); setTimeout('next()',1000);}";
static const char* scrollRightScript = "window.scrollTo(50,0);";
static const char* scrollLeftScript = "window.scrollTo(50,0); setTimeout(next, 500); function next(){ window.scrollTo(0,0); }";

typedef void (*Scroll_Callback)();
static Scroll_Callback finishedCallback;

extern struct Ewk_Test_View test_view;

class utc_blink_ewk_scroll_notifications : public ::testing::Test {
  protected:
    utc_blink_ewk_scroll_notifications() {
    }

    virtual ~utc_blink_ewk_scroll_notifications() {
    }

    virtual void SetUp() {
      utc_blink_ewk_test_init();
      evas_object_smart_callback_add(test_view.webview, "load,finished", load_finished, NULL);
      evas_object_smart_callback_add(test_view.webview, "load,error", load_error, NULL);
      evas_object_smart_callback_add(test_view.webview, "scroll,down", scroll_down, NULL);
      evas_object_smart_callback_add(test_view.webview, "scroll,up", scroll_up, NULL);
      evas_object_smart_callback_add(test_view.webview, "scroll,right", scroll_right, NULL);
      evas_object_smart_callback_add(test_view.webview, "scroll,left", scroll_left, NULL);
    }

    virtual void TearDown() {
      evas_object_smart_callback_del(test_view.webview, "scroll,down", scroll_down);
      evas_object_smart_callback_del(test_view.webview, "scroll,up", scroll_up);
      evas_object_smart_callback_del(test_view.webview, "scroll,right", scroll_right);
      evas_object_smart_callback_del(test_view.webview, "scroll,left", scroll_left);
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

    static void scroll_down(void* data, Evas_Object* webview, void* event_info)
    {
      if (!did_scroll_down) {
        utc_message("Reached scrolldown\n");
        did_scroll_down = EINA_TRUE;
        utc_blink_main_loop_quit();
      }
    }

    static void scroll_up(void* data, Evas_Object* webview, void* event_info)
    {
      utc_message("Reached scrollup\n");
      did_scroll_up = EINA_TRUE;
      utc_blink_main_loop_quit();
    }

    static void scroll_left(void* data, Evas_Object* webview, void* event_info)
    {
      did_scroll_left = EINA_TRUE;
      utc_blink_main_loop_quit();
    }

    static void scroll_right(void* data, Evas_Object* webview, void* event_info)
    {
      if (!did_scroll_right) {
       did_scroll_right = EINA_TRUE;
       utc_blink_main_loop_quit();
      }
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
* @brief Tests if scroll down notification was invoked
*/
TEST_F(utc_blink_ewk_scroll_notifications, POS_TEST1)
{
  loadURL(SAMPLE_HTML_FILE_UP_DOWN);
  utc_blink_main_loop_begin();
  utc_check_eq(did_scroll_down, EINA_TRUE);
}

/**
* @brief Tests if scroll up notification was invoked
*/
TEST_F(utc_blink_ewk_scroll_notifications, POS_TEST2)
{
  did_scroll_down = EINA_TRUE;
  loadURL(SAMPLE_HTML_FILE_UP_DOWN);
  utc_blink_main_loop_begin();
  utc_check_eq(did_scroll_up, EINA_TRUE);
}

/**
* @brief Tests if scroll right notification was invoked
*/
TEST_F(utc_blink_ewk_scroll_notifications, POS_TEST3)
{
  loadURL(SAMPLE_HTML_FILE_LEFT_RIGHT);
  utc_blink_main_loop_begin();
  utc_check_eq(did_scroll_right, EINA_TRUE);
}

/**
* @brief Tests if scroll left notification was invoked
*/
TEST_F(utc_blink_ewk_scroll_notifications, POS_TEST4)
{
  did_scroll_right = EINA_TRUE;
  loadURL(SAMPLE_HTML_FILE_LEFT_RIGHT);
  did_scroll_right = EINA_TRUE;
  utc_blink_main_loop_begin();
  utc_check_eq(did_scroll_left, EINA_TRUE);
}