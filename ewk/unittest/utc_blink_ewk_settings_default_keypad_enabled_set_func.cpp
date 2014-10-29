// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_settings_default_keypad_enabled_set : public utc_blink_ewk_base {
};

/**
 * @brief Tests if returns TRUE when initiated with a correct webview and
 * default keypad enabled.
 */
TEST_F(utc_blink_ewk_settings_default_keypad_enabled_set, Set)
{
  Ewk_Settings* settings = ewk_view_settings_get(GetEwkWebView());
  ASSERT_TRUE(NULL != settings);

  Eina_Bool set = !ewk_settings_default_keypad_enabled_get(settings);
  ASSERT_EQ(EINA_TRUE, ewk_settings_default_keypad_enabled_set(settings, set));
  ASSERT_EQ(set, ewk_settings_default_keypad_enabled_get(settings));

  set = !set;
  ASSERT_EQ(EINA_TRUE, ewk_settings_default_keypad_enabled_set(settings, set));
  ASSERT_EQ(set, ewk_settings_default_keypad_enabled_get(settings));
}

/**
 * @brief Tests if returns FALSE when initiated with NULL webview.
 */
TEST_F(utc_blink_ewk_settings_default_keypad_enabled_set, NullArg)
{
  ASSERT_NE(EINA_TRUE, ewk_settings_default_keypad_enabled_set(NULL, EINA_FALSE));
}
