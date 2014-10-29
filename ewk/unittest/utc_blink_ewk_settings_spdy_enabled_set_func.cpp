// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_settings_spdy_enabled_set : public utc_blink_ewk_base {
};

/**
* @brief Tests if returns TRUE when initiated with correct settings and
* enabled spdy set to TRUE.
*/
TEST_F(utc_blink_ewk_settings_spdy_enabled_set, POS_TEST)
{
  Ewk_Settings* settings = ewk_view_settings_get(GetEwkWebView());
  ASSERT_TRUE(settings);

  Eina_Bool result = ewk_settings_spdy_enabled_set(settings, EINA_TRUE);
  ASSERT_EQ(EINA_TRUE, result);
}

/**
* @brief Tests if returns FALSE when initiated with NULL settings.
*/
TEST_F(utc_blink_ewk_settings_spdy_enabled_set, NEG_TEST)
{
  Eina_Bool result = ewk_settings_spdy_enabled_set(NULL, EINA_FALSE);
  EXPECT_NE(result, EINA_TRUE);
}
