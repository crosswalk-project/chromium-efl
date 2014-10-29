// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_view_settings_get : public utc_blink_ewk_base {

};

/**
 * @brief Positive test case of ewk_view_settings_get().
 */
TEST_F(utc_blink_ewk_view_settings_get, POS_TEST)
{
  Ewk_Settings* settings = ewk_view_settings_get(GetEwkWebView());
  if (!settings)
    FAIL();
}

/**
 * @brief  checking whether function is working when providing NULL value.
 */
TEST_F(utc_blink_ewk_view_settings_get, NEG_TEST)
{
  Ewk_Settings* settings = ewk_view_settings_get(NULL);
  if (settings)
    FAIL();
}
