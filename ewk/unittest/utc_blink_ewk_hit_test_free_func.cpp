// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_hit_test_free : public utc_blink_ewk_base {
protected:

  void LoadFinished(Evas_Object* webview)
  {
    EventLoopStop(utc_blink_ewk_base::Success);
  }

  static const char* const test_url;
};

const char* const utc_blink_ewk_hit_test_free::test_url = "http://m.naver.com";

/**
 * @brief Checking whether the hit test instance is freed properly.
 */
TEST_F(utc_blink_ewk_hit_test_free, POS_TEST1)
{
  Eina_Bool result = ewk_view_url_set(GetEwkWebView(), test_url);
  if (!result)
    utc_fail();
  utc_blink_ewk_base::MainLoopResult loop_result = EventLoopStart();

  if (loop_result != utc_blink_ewk_base::Success) {
    utc_fail();
  }

  result = EINA_TRUE;
  Ewk_Hit_Test* hit_test = ewk_view_hit_test_new(GetEwkWebView(), 200, 200, EWK_HIT_TEST_MODE_DEFAULT);
  ewk_hit_test_free(hit_test);

  hit_test = ewk_view_hit_test_new(GetEwkWebView(), 200, 200, EWK_HIT_TEST_MODE_NODE_DATA);
  ewk_hit_test_free(hit_test);

  hit_test = ewk_view_hit_test_new(GetEwkWebView(), 200, 200, EWK_HIT_TEST_MODE_IMAGE_DATA);
  ewk_hit_test_free(hit_test);

  hit_test = ewk_view_hit_test_new(GetEwkWebView(), 200, 200, EWK_HIT_TEST_MODE_ALL);
  ewk_hit_test_free(hit_test);

  utc_check_eq(result, EINA_TRUE);
}

/**
 * @brief Checking whether function works properly in case of NULL of a hit test instance.
 */
TEST_F(utc_blink_ewk_hit_test_free, NEG_TEST1)
{
  Eina_Bool result = EINA_TRUE;

  ewk_hit_test_free(NULL);
  utc_check_eq(result, EINA_TRUE);
}
