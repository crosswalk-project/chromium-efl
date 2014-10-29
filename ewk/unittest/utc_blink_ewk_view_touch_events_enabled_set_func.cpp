// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_view_touch_events_enabled_set : public utc_blink_ewk_base {
};

/**
 * @brief Checking whether the state of mouse events behavior is set properly.
 */
TEST_F(utc_blink_ewk_view_touch_events_enabled_set, POS_TEST)
{
  ewk_view_touch_events_enabled_set(GetEwkWebView(), EINA_TRUE);
  Eina_Bool enable_get = ewk_view_touch_events_enabled_get(GetEwkWebView());
  if (enable_get != EINA_TRUE) {
    FAIL();
  }

  ewk_view_touch_events_enabled_set(GetEwkWebView(), EINA_FALSE);
  enable_get = ewk_view_touch_events_enabled_get(GetEwkWebView());
  if (enable_get != EINA_FALSE) {
    FAIL();
  }
}

/**
 * @brief There is no negative case for ewk_view_mouse_events_enabled_set function.
 */
TEST_F(utc_blink_ewk_view_touch_events_enabled_set, NEG_TEST)
{
  ewk_view_touch_events_enabled_set(NULL, EINA_FALSE);
  ewk_view_touch_events_enabled_set(NULL, EINA_TRUE);

  SUCCEED();
}

