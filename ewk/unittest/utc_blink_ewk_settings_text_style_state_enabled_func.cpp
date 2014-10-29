// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_settings_text_style_state_enabled : public utc_blink_ewk_base {
};

/**
 * @brief Tests if returns TRUE when initiated with a correct webview and
 * enabled text selection set to TRUE.
 */
TEST_F(utc_blink_ewk_settings_text_style_state_enabled, POS_TEST1)
{
  Ewk_Settings* settings = ewk_view_settings_get(GetEwkWebView());
  if (!settings) {
    FAIL();
  }

  ewk_settings_text_style_state_enabled_set(settings, EINA_TRUE);
  Eina_Bool result = ewk_settings_text_style_state_enabled_get(settings);
  if (!result) {
    FAIL();
  }
  EXPECT_EQ(result, EINA_TRUE);
}

/**
 * @brief Tests if returns TRUE when initiated with a correct webview and
 * enabled text selection set to FALSE.
 */
TEST_F(utc_blink_ewk_settings_text_style_state_enabled, POS_TEST2)
{
  Ewk_Settings* settings = ewk_view_settings_get(GetEwkWebView());
  if (!settings) {
    FAIL();
  }

  ewk_settings_text_style_state_enabled_set(settings, EINA_FALSE);
  Eina_Bool result = ewk_settings_text_style_state_enabled_get(settings);
  if (result) {
    FAIL();
  }
  EXPECT_EQ(result, EINA_FALSE);
}

/**
 * @brief Tests if returns FALSE when initiated with NULL webview.
 */
TEST_F(utc_blink_ewk_settings_text_style_state_enabled, NEG_TEST)
{
  ewk_settings_text_style_state_enabled_set(NULL, EINA_FALSE);
  SUCCEED();
}
