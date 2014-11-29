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
#define TESTED_FUN_NAME ewk_view_feed_touch_event
#define POSITIVE_TEST_FUN_NUM 2
#define NEGATIVE_TEST_FUN_NUM 1

#include "utc_blink_ewk.h"
#include "ewk_touch.h"

#ifndef NULL
#define NULL 0x0
#endif

#define RESOURCE_PATH "/ewk_view/index_big_red_square.html"

extern struct Ewk_Test_View test_view;

class utc_blink_ewk_view_feed_touch_event : public ::testing::Test
{
 protected:
  utc_blink_ewk_view_feed_touch_event()
  {
  }

  ~utc_blink_ewk_view_feed_touch_event()
  {
  }

  static void loadFinished(void* data, Evas_Object* eObject, void* dataFinished)
  {
    utc_message("[loadFinished] :: \n");
  }

  /* Callback for load error */
  static void loadError(void* data, Evas_Object* webview, void* event_info)
  {
    utc_message("[load_error] :: \n");
    utc_blink_main_loop_quit();

    utc_fail();
  }

  static void frameRendered(void* data, Evas_Object* eObject, void* dataFinished)
  {
    utc_message("[frameRendered] :: \n");
    utc_blink_main_loop_quit();
  }

 /* Startup function */
  void SetUp()
  {
    utc_blink_ewk_test_init();
    evas_object_smart_callback_add(test_view.webview, "load,finished", loadFinished, NULL);
    evas_object_smart_callback_add(test_view.webview, "load,error", loadError, NULL);
    evas_object_smart_callback_add(test_view.webview, "frame,rendered", frameRendered, NULL);
  }

  /* Cleanup function */
  void TearDown()
  {
    evas_object_smart_callback_del(test_view.webview, "load,finished", loadFinished);
    evas_object_smart_callback_del(test_view.webview, "load,error", loadError);
    evas_object_smart_callback_del(test_view.webview, "frame,rendered", frameRendered);
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
 * @brief Positive test case of ewk_view_feed_touch_event(), testing start of touch.
 */

TEST_F(utc_blink_ewk_view_feed_touch_event, POS_TEST1)
{
  loadURL(RESOURCE_PATH);
  utc_blink_main_loop_begin();

  Eina_List* points = 0;
  unsigned i = 0;
  for (i = 0; i < 2; ++i) {
    Ewk_Touch_Point* point = (Ewk_Touch_Point*)calloc(sizeof(Ewk_Touch_Point), 1);
    point->id = i;
    point->x = 10;
    point->y = 10;
    point->state = EVAS_TOUCH_POINT_DOWN;
    points = eina_list_append(points, point);
  }

  Eina_Bool result = ewk_view_feed_touch_event(test_view.webview, EWK_TOUCH_START, points, evas_key_modifier_get(evas_object_evas_get(test_view.webview)));

  void* data;
  EINA_LIST_FREE(points, data)
  free((Ewk_Touch_Point*)data);

  utc_check_eq(result, EINA_TRUE);
}

/**
 * @brief Positive test case of ewk_view_feed_touch_event(), testing end of touch.
 */

TEST_F(utc_blink_ewk_view_feed_touch_event, POS_TEST2)
{
  Eina_List* points = 0;
  unsigned i = 0;
  for (i = 0; i < 2; ++i) {
    Ewk_Touch_Point* point = (Ewk_Touch_Point*)calloc(sizeof(Ewk_Touch_Point), 1);
    point->id = i;
    point->x = 10;
    point->y = 10;
    point->state = EVAS_TOUCH_POINT_UP;
    points = eina_list_append(points, point);
  }

  Eina_Bool result = ewk_view_feed_touch_event(test_view.webview, EWK_TOUCH_END, points, evas_key_modifier_get(evas_object_evas_get(test_view.webview)));

  void* data;
  EINA_LIST_FREE(points, data)
    free((Ewk_Touch_Point*)data);

  utc_check_eq(result, EINA_TRUE);
}

/**
 * @brief Negative test case of ewk_view_feed_touch_event(), testing for null
 */
TEST_F(utc_blink_ewk_view_feed_touch_event, NEG_TEST)
{
  utc_check_eq(ewk_view_feed_touch_event(0, EWK_TOUCH_END, 0, 0), EINA_FALSE);
}
