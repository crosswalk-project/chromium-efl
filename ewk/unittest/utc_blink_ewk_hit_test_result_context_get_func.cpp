// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_hit_test_result_context_get : public utc_blink_ewk_base {
protected:
  void LoadFinished(Evas_Object* webview)
  {
    EventLoopStop(utc_blink_ewk_base::Success);
  }

  static const char* const test_path;
};

const char* const utc_blink_ewk_hit_test_result_context_get::test_path = "/ewk_hit_test/index.html";

/**
 * @brief Checking whether the context of the hit test is returned properly.
 */
TEST_F(utc_blink_ewk_hit_test_result_context_get, POS_TEST1)
{
  bool result = ewk_view_url_set(GetEwkWebView(), GetResourceUrl(test_path).c_str());
  if (!result)
    utc_fail();
  utc_blink_ewk_base::MainLoopResult loop_result = EventLoopStart();

  if (loop_result != utc_blink_ewk_base::Success) {
    utc_fail();
  }

  Ewk_Hit_Test* hit_test = ewk_view_hit_test_new(GetEwkWebView(), 200, 200, EWK_HIT_TEST_MODE_DEFAULT);
  if (ewk_hit_test_result_context_get(hit_test) & EWK_HIT_TEST_RESULT_CONTEXT_LINK &&
      ewk_hit_test_result_context_get(hit_test) & EWK_HIT_TEST_RESULT_CONTEXT_IMAGE) {
    result = true;
  } else {
      result = false;
  }

  ewk_hit_test_free(hit_test);
  utc_check_true(result);
}

/**
 * @brief Checking whether function works properly in case of NULL of a hit test instance.
 */
TEST_F(utc_blink_ewk_hit_test_result_context_get, NEG_TEST1)
{
  utc_check_eq(ewk_hit_test_result_context_get(NULL), EWK_HIT_TEST_RESULT_CONTEXT_DOCUMENT);
}
