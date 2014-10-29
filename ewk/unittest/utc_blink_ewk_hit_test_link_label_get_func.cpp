// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_hit_test_link_label_get : public utc_blink_ewk_base {
protected:
  void LoadFinished(Evas_Object* webview)
  {
    EventLoopStop(utc_blink_ewk_base::Success);
  }

  static const char* const test_path;
};

const char* const utc_blink_ewk_hit_test_link_label_get::test_path = "/ewk_hit_test/index.html";

/**
 * @brief Checking whether the link label of hit element is returned properly.
 */
TEST_F(utc_blink_ewk_hit_test_link_label_get, POS_TEST1)
{
  bool result = ewk_view_url_set(GetEwkWebView(), GetResourceUrl(test_path).c_str());
  if (!result)
    utc_fail();
  utc_blink_ewk_base::MainLoopResult loop_result = EventLoopStart();

  if (loop_result != utc_blink_ewk_base::Success) {
    utc_fail();
  }

  Ewk_Hit_Test* hit_test = ewk_view_hit_test_new(GetEwkWebView(), 80, 100, EWK_HIT_TEST_MODE_DEFAULT);
  if (ewk_hit_test_link_label_get(hit_test))
    result = EINA_TRUE;
  else
    result = EINA_FALSE;

  utc_check_eq(EINA_TRUE, result);
  ewk_hit_test_free(hit_test);
}

/**
 * @brief Checking whether function works properly in case of NULL of a hit test instance.
 */
TEST_F(utc_blink_ewk_hit_test_link_label_get, NEG_TEST1)
{
  utc_check_eq(0, ewk_hit_test_link_label_get(NULL));
}
