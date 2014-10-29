// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_error_type_get : public utc_blink_ewk_base {
 protected:
  utc_blink_ewk_error_type_get()
    : error_type(EWK_ERROR_TYPE_NONE)
  {
  }

  /* Callback for load error */
  bool LoadError(Evas_Object* webview, Ewk_Error* error)
  {
    utc_message("[load_error] :: ");
    error_type = ewk_error_type_get(error);
    EventLoopStop(utc_blink_ewk_base::Success);
    return true;
  }

  /* Callback for load progress */
  void LoadProgress(Evas_Object* webview, double progress)
  {
    ewk_view_stop(webview);
  }

protected:
  Ewk_Error_Type error_type;
  static const char* const test_url;
};

const char* const utc_blink_ewk_error_type_get::test_url = "http://google.com";

/**
 * @brief Positive test case of ewk_error_type_get(). Page is loaded and stopped in between to generate loadError
 */
TEST_F(utc_blink_ewk_error_type_get, POS_TEST)
{
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), test_url));
  ASSERT_EQ(utc_blink_ewk_base::Success, EventLoopStart());
  ASSERT_EQ(EWK_ERROR_TYPE_NETWORK, error_type); // right now this is the only error type supported
}

/**
* @brief Checking whether function works properly in case of NULL argument.
*/
TEST_F(utc_blink_ewk_error_type_get, NEG_TEST)
{
  ASSERT_EQ(EWK_ERROR_TYPE_NONE, ewk_error_type_get(NULL));
}
