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

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_hit_test_attribute_hash_get : public utc_blink_ewk_base {
protected:
  void LoadFinished(Evas_Object* webview)
  {
    EventLoopStop(utc_blink_ewk_base::Success);
  }

  static const char* const test_path;
};

const char* const utc_blink_ewk_hit_test_attribute_hash_get::test_path = "/ewk_hit_test/index.html";

/**
 * @brief Checking whether the attribute data of hit element is returned properly.
 */
TEST_F(utc_blink_ewk_hit_test_attribute_hash_get, POS_TEST1)
{
  bool result = ewk_view_url_set(GetEwkWebView(), GetResourceUrl(test_path).c_str());
  if (!result)
    utc_fail();
  utc_blink_ewk_base::MainLoopResult loop_result = EventLoopStart();

  if (loop_result != utc_blink_ewk_base::Success) {
    utc_fail();
  }

  Ewk_Hit_Test* hit_test = ewk_view_hit_test_new(GetEwkWebView(), 200, 200, EWK_HIT_TEST_MODE_NODE_DATA);
  if (ewk_hit_test_attribute_hash_get(hit_test))
    result = true;
  else
    result = false;

  ewk_hit_test_free(hit_test);
  utc_check_true(result);
}

/**
 * @brief Checking whether function works properly in case of NULL of a hit test instance.
 */
TEST_F(utc_blink_ewk_hit_test_attribute_hash_get, NEG_TEST1)
{
  utc_check_eq(0, ewk_hit_test_attribute_hash_get(NULL));
}
