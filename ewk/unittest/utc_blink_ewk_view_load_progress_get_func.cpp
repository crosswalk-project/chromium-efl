/*
 * Chromium EFL
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

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_view_load_progress_get : public utc_blink_ewk_base {
protected:

  void LoadProgress(Evas_Object* webview, double progress)
  {
    EventLoopStop(utc_blink_ewk_base::Success);
  }

protected:
  static const char * const SAMPLE_HTML_FILE;
  static const double cProgressComplete;
  static const double cInvlaidLoad;
};

const char * const utc_blink_ewk_view_load_progress_get::SAMPLE_HTML_FILE = "/common/sample.html";
const double utc_blink_ewk_view_load_progress_get::cProgressComplete = 1.0;
const double utc_blink_ewk_view_load_progress_get::cInvlaidLoad = -1.0;

/**
  * @brief Tests if returns TRUE when initiated with a correct context without a page load.
  */
TEST_F(utc_blink_ewk_view_load_progress_get, POS_TEST1)
{
  Eina_Bool result = EINA_FALSE;
  double value = ewk_view_load_progress_get(GetEwkWebView());
  if (value == 0) {
    result = EINA_TRUE;
  }
  evas_object_show(GetEwkWebView());
  evas_object_show(GetEwkWebView());
  EXPECT_EQ(result, EINA_TRUE);
}

/**
  * @brief Tests if returns TRUE when not initiated with correct context with a valid page load.
  */
TEST_F(utc_blink_ewk_view_load_progress_get, POS_TEST2)
{
  ewk_view_url_set(GetEwkWebView(), GetResourceUrl(SAMPLE_HTML_FILE).c_str());

  utc_blink_ewk_base::MainLoopResult main_result = EventLoopStart();
  if (main_result != utc_blink_ewk_base::Success) {
    FAIL();
  }

  Eina_Bool result = EINA_FALSE;
  double value = ewk_view_load_progress_get(GetEwkWebView());
  if (value <= cProgressComplete) {
    result = EINA_TRUE;
  }
  evas_object_show(GetEwkWebView());
  evas_object_show(GetEwkWebView());
  EXPECT_EQ(result, EINA_TRUE);
}

/**
  * @brief Tests if returns FALSE when not initiated with NULL context.
  */
TEST_F(utc_blink_ewk_view_load_progress_get, NEG_TEST)
{
  Eina_Bool result = EINA_FALSE;
  double value = ewk_view_load_progress_get(NULL);
  if (value == cInvlaidLoad) {
     result = EINA_TRUE;
  }
  evas_object_show(GetEwkWebView());
  evas_object_show(GetEwkWebView());
  EXPECT_EQ(result, EINA_TRUE);
}