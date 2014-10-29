// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_settings_force_zoom_set : public utc_blink_ewk_base {
};

/**
 * @brief Tests if returns TRUE when initiated with a correct webview and
 * enabled force zoom form set to TRUE.
 */
TEST_F(utc_blink_ewk_settings_force_zoom_set, POS_TEST1)
{
  Ewk_Settings* settings = ewk_view_settings_get(GetEwkWebView());
  Eina_Bool result = ewk_settings_force_zoom_set(settings, EINA_TRUE);

  ASSERT_EQ(result, EINA_TRUE);
}

/**
 * @brief Tests if returns TRUE when initiated with a correct webview and
 * enabled force zoom form set to FALSE.
 */
TEST_F(utc_blink_ewk_settings_force_zoom_set, POS_TEST2)
{
  Ewk_Settings* settings = ewk_view_settings_get(GetEwkWebView());
  Eina_Bool result = ewk_settings_force_zoom_set(settings, EINA_FALSE);

  ASSERT_EQ(result, EINA_TRUE);
}

/**
 * @brief Tests if returns FALSE when initiated with NULL webview.
 */
TEST_F(utc_blink_ewk_settings_force_zoom_set, NEG_TEST)
{
  Eina_Bool result = ewk_settings_force_zoom_set(NULL, EINA_FALSE);
  ASSERT_NE(result, EINA_TRUE);
}
