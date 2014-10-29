// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_settings_editable_link_behavior_set : public utc_blink_ewk_base {
};

/**
 * @brief Tests if returns TRUE when initiated with a correct webview and set to EWK_EDITABLE_LINK_BEHAVIOR_DEFAULT.
 */
TEST_F(utc_blink_ewk_settings_editable_link_behavior_set, POS_TEST)
{
  Ewk_Settings* settings = ewk_view_settings_get(GetEwkWebView());
  if (!settings) {
    FAIL();
  }
  Eina_Bool result = ewk_settings_editable_link_behavior_set(settings, EWK_EDITABLE_LINK_BEHAVIOR_DEFAULT);
  if (!result) {
    FAIL();
  }
  result = ewk_settings_editable_link_behavior_set(settings, EWK_EDITABLE_LINK_BEHAVIOR_ALWAYS_LIVE);
  if (!result) {
    FAIL();
  }
  result = ewk_settings_editable_link_behavior_set(settings, EWK_EDITABLE_LINK_BEHAVIOR_ONLY_LIVE_WITH_SHIFTKEY);
  if (!result) {
    FAIL();
  }
  result = ewk_settings_editable_link_behavior_set(settings, EWK_EDITABLE_LINK_BEHAVIOR_LIVE_WHEN_NOT_FOCUSED);
  if (!result) {
    FAIL();
  }
  result = ewk_settings_editable_link_behavior_set(settings, EWK_EDITABLE_LINK_BEHAVIOR_NEVER_LIVE);
  if (!result) {
    FAIL();
  }
  EXPECT_EQ(result, EINA_TRUE);
}

/**
 * @brief Tests if returns FALSE when initiated with NULL webview.
 */
TEST_F(utc_blink_ewk_settings_editable_link_behavior_set, NEG_TEST)
{
  Eina_Bool result = ewk_settings_editable_link_behavior_set(NULL, EWK_EDITABLE_LINK_BEHAVIOR_DEFAULT);
  EXPECT_EQ(result, EINA_FALSE);
}
