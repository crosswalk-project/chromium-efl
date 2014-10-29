// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_settings_auto_fitting_get : public utc_blink_ewk_base
{
};

/**
 * @brief Positive test case of ewk_settings_auto_fitting_get()
 */
TEST_F(utc_blink_ewk_settings_auto_fitting_get, Change)
{
  Ewk_Settings* const settings = ewk_view_settings_get(GetEwkWebView());
  ASSERT_TRUE(settings);

  Eina_Bool const fitting = !ewk_settings_auto_fitting_get(settings);
  ASSERT_EQ(EINA_TRUE, ewk_settings_auto_fitting_set(settings, fitting));
  EXPECT_EQ(fitting, ewk_settings_auto_fitting_get(settings));
}

/**
 * @brief Negative test case of ewk_settings_auto_fitting_get()
 */
TEST_F(utc_blink_ewk_settings_auto_fitting_get, NullArg)
{
  EXPECT_EQ(EINA_FALSE, ewk_settings_auto_fitting_get(NULL));
}
