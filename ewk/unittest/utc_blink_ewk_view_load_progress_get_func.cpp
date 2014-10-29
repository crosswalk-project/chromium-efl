// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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