// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

#define SAMPLE_HTML_FILE ("/common/sample.html")
#define TITLE0 "Test file"

class utc_blink_ewk_view_title_get_func : public utc_blink_ewk_base
{
protected:
  void LoadFinished(Evas_Object*)
  {
    EventLoopStop(utc_blink_ewk_base::Success); // won't fail the test if EventLoopStop was already called
  }
};

/**
 * @brief Positive test case of ewk_view_title_get
 */
TEST_F(utc_blink_ewk_view_title_get_func, POS_TEST)
{
    Eina_Bool result = ewk_view_url_set(GetEwkWebView(), GetResourceUrl("common/sample.html").c_str());
    if (!result)
        utc_fail();
    if (EventLoopStart() != utc_blink_ewk_base::Success)
        utc_fail();
    char* title = strdup(ewk_view_title_get(GetEwkWebView()));
    utc_message("Current title: %s", title);
    utc_check_str_eq(title, TITLE0);
    free(title);
}

/**
 * @brief Negative test case of ewk_view_title_get
 */
TEST_F(utc_blink_ewk_view_title_get_func, NEG_TEST)
{
  const char* title = ewk_view_title_get(0);
  if (title)
    utc_fail();
}
