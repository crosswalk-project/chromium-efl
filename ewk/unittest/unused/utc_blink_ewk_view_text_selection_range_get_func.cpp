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

#ifndef NULL
#define NULL    0x0
#endif
#define SAMPLE_HTML_FILE                ("/selection/sample_1.html")

extern struct Ewk_Test_View test_view;

class utc_blink_ewk_view_text_selection_range_get : public ::testing::Test {
protected:
  utc_blink_ewk_view_text_selection_range_get() {
  }

  virtual ~utc_blink_ewk_view_text_selection_range_get() {
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

  /*Callback for Load Error */
  static void loadError(void *data, Evas_Object *webview, void *error)
  {
    utc_message("[loadError] :: \n");
    utc_blink_main_loop_quit();
  }

  /*Callback for Load finished */
  static void loadFinished(void *data, Evas_Object *webview, void *arg)
  {
    utc_message("[loadFinished] :: \n");
    utc_blink_main_loop_quit();
  }

  /* Startup and cleanup functions */
  virtual void SetUp()
  {
    utc_blink_ewk_test_init();
    evas_object_smart_callback_add(test_view.webview, "load,error", loadError,NULL);
    evas_object_smart_callback_add(test_view.webview, "load,finished", loadFinished,NULL);
  }

  virtual void TearDown()
  {
    evas_object_smart_callback_del(test_view.webview, "load,error", loadError);
    evas_object_smart_callback_del(test_view.webview, "load,finished", loadFinished);
    utc_blink_ewk_test_end();
  }
};

/**
 * @brief Positive test case of ewk_view_text_selection_range_get(),testing results to get selected Range.
 */
TEST_F(utc_blink_ewk_view_text_selection_range_get, POS_TEST)
{
  loadURL(SAMPLE_HTML_FILE);
  utc_blink_main_loop_begin();

  const char textRangeHTML[] =
     "<!DOCTYPE html>"
      "<html><head>"
      "<script>"
      "window.onload = function(){"
      "var text_area = document.getElementById('txtarea');"
      "text_area.focus();"
      "text_area.select();}"
      "</script></head>"
      "<body>"
      "<textarea id='txtarea'>select text</textarea>"
      "</body></html>";

  ewk_view_html_string_load(test_view.webview, textRangeHTML, 0, 0);

  // Wait until load finished.
  utc_blink_main_loop_begin();

  Eina_Rectangle leftrect, rightrect;
  Eina_Bool result = ewk_view_text_selection_range_get(test_view.webview, &leftrect, &rightrect);
  utc_check_eq(result, EINA_TRUE);
}

/**
 * @brief  checking whether function is working when providing NULL value.
 */
TEST_F(utc_blink_ewk_view_text_selection_range_get, NEG_TEST)
{
  Eina_Rectangle leftrect, rightrect;
  Eina_Bool result = ewk_view_text_selection_range_get(NULL, &leftrect, &rightrect);
  utc_check_eq(result, EINA_FALSE);
}
