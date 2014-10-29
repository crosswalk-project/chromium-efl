// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_settings_autofill_password_form_enabled_get : public utc_blink_ewk_base
{
};


/**
 * @brief Tests if returns TRUE when initiated with a correct webview and
 * enabled autofill password form set to TRUE.
 */
TEST_F(utc_blink_ewk_settings_autofill_password_form_enabled_get, POS_TEST1)
{
  Ewk_Settings* settings = ewk_view_settings_get(GetEwkWebView());
  ASSERT_TRUE(settings);
  ASSERT_TRUE(ewk_settings_autofill_password_form_enabled_set(settings, EINA_TRUE));
  ASSERT_TRUE(ewk_settings_autofill_password_form_enabled_get(settings));
}

/**
 * @brief Tests if returns TRUE when initiated with a correct webview and
 * enabled autofill password form set to FALSE.
 */
TEST_F(utc_blink_ewk_settings_autofill_password_form_enabled_get, POS_TEST2)
{
  Ewk_Settings* settings = ewk_view_settings_get(GetEwkWebView());
  ASSERT_TRUE(settings);
  ASSERT_TRUE(ewk_settings_autofill_password_form_enabled_set(settings, EINA_FALSE));
  ASSERT_FALSE(ewk_settings_autofill_password_form_enabled_get(settings));
}

/**
 * @brief Tests if returns FALSE when initiated with NULL webview.
 */
TEST_F(utc_blink_ewk_settings_autofill_password_form_enabled_get, NEG_TEST)
{
  ASSERT_FALSE(ewk_settings_autofill_password_form_enabled_get(0));
}
