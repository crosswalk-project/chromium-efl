// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_settings_autofill_password_form_enabled_set : public utc_blink_ewk_base {
};


/**
 * @brief Tests if returns TRUE when initiated with a correct webview and
 * enabled autofill password form set to TRUE.
 */
TEST_F(utc_blink_ewk_settings_autofill_password_form_enabled_set, POS_TEST1)
{
  Ewk_Settings* settings = ewk_view_settings_get(GetEwkWebView());
  if (!settings) {
    FAIL();
  }

  Eina_Bool result = ewk_settings_autofill_password_form_enabled_set(settings, EINA_TRUE);
  if (!result) {
    FAIL();
  }
  EXPECT_EQ(result, EINA_TRUE);
}

/**
 * @brief Tests if returns TRUE when initiated with a correct webview and
 * enabled autofill password form set to FALSE.
 */
TEST_F(utc_blink_ewk_settings_autofill_password_form_enabled_set, POS_TEST2)
{
  Ewk_Settings* settings = ewk_view_settings_get(GetEwkWebView());
  if (!settings) {
    FAIL();
  }

  Eina_Bool result = ewk_settings_autofill_password_form_enabled_set(settings, EINA_FALSE);
  if (!result) {
    FAIL();
  }
  EXPECT_EQ(result, EINA_TRUE);
}

/**
 * @brief Tests if returns FALSE when initiated with NULL webview.
 */
TEST_F(utc_blink_ewk_settings_autofill_password_form_enabled_set, NEG_TEST)
{
  Eina_Bool result = ewk_settings_autofill_password_form_enabled_set(NULL, EINA_FALSE);
  EXPECT_NE(result, EINA_TRUE);
}
