// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_settings_initial_list_style_position_get : public utc_blink_ewk_base {
};


/**
* @brief checking if returns EWK_LIST_STYLE_POSITION_OUTSIDE when initiated with a correct webview and
* initial list style position set to EWK_LIST_STYLE_POSITION_OUTSIDE.
*/
TEST_F(utc_blink_ewk_settings_initial_list_style_position_get, POS_TEST1)
{
  Ewk_Settings *settings = ewk_view_settings_get(GetEwkWebView());
  if (!settings) {
    FAIL();
  }

  Eina_Bool result = ewk_settings_initial_list_style_position_set(settings, EWK_LIST_STYLE_POSITION_OUTSIDE);
  if (!result) {
    FAIL();
  }
  EXPECT_EQ(ewk_settings_initial_list_style_position_get(settings), EWK_LIST_STYLE_POSITION_OUTSIDE);
}

/**
* @brief checking if returns EWK_LIST_STYLE_POSITION_INSIDE when initiated with a correct webview and
* initial list style position set to EWK_LIST_STYLE_POSITION_INSIDE.
*/
TEST_F(utc_blink_ewk_settings_initial_list_style_position_get, POS_TEST2)
{
  Ewk_Settings *settings = ewk_view_settings_get(GetEwkWebView());
  if (!settings) {
    FAIL();
  }

  Eina_Bool result = ewk_settings_initial_list_style_position_set(settings, EWK_LIST_STYLE_POSITION_INSIDE);
  if (!result) {
    FAIL();
  }
  EXPECT_EQ(ewk_settings_initial_list_style_position_get(settings), EWK_LIST_STYLE_POSITION_INSIDE);
}

/**
* @brief checking if returns EWK_LIST_STYLE_POSITION_OUTSIDE when initiated with NULL webview.
*/
TEST_F(utc_blink_ewk_settings_initial_list_style_position_get, NEG_TEST)
{
  Ewk_List_Style_Position result = ewk_settings_initial_list_style_position_get(NULL);
  EXPECT_EQ(result, EWK_LIST_STYLE_POSITION_OUTSIDE);
}
