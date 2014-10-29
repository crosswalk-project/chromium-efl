// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_error_description_get : public utc_blink_ewk_base {
protected:
  /* Callback for load error */
  bool LoadError(Evas_Object* webview, Ewk_Error* error)
  {
    utc_message("[load_error] :: ");

    if(ewk_error_description_get(error)) {
      EventLoopStop(utc_blink_ewk_base::Success);
      return true; // handled load error, supress default behaviour
    }

    return false; // fallback default behaviour
  }

  /* Callback for load progress */
  void LoadProgress(Evas_Object *webview, double progress)
  {
    ewk_view_stop(webview);
  }

protected:
  static const char* const test_url;
};

const char* const utc_blink_ewk_error_description_get::test_url = "http://google.com";
/**
 * @brief Positive test case of ewk_error_description_get(). Page is loaded and stopped in between to generate loadError
 */
TEST_F(utc_blink_ewk_error_description_get, POS_TEST)
{
  Eina_Bool result = ewk_view_url_set(GetEwkWebView(), test_url);
  if (!result) {
    utc_fail();
  }

  utc_blink_ewk_base::MainLoopResult loop_result = EventLoopStart();
  utc_check_eq(utc_blink_ewk_base::Success, loop_result);
}

/**
* @brief Checking whether function works properly in case of NULL argument.
*/
TEST_F(utc_blink_ewk_error_description_get, NEG_TEST)
{
  const char* description = ewk_error_description_get(NULL);
  utc_check_eq(NULL, description);
}
