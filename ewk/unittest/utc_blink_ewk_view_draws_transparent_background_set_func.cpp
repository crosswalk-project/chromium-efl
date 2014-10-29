// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_view_draws_transparent_background_set : public utc_blink_ewk_base
{
};

/**
* @brief checking if returns TRUE when initiated with a correct webview and
* enabled draws transparent background set to TRUE.
*/
TEST_F(utc_blink_ewk_view_draws_transparent_background_set, POS_TEST1)
{
  Eina_Bool result = ewk_view_draws_transparent_background_set(GetEwkWebView(), EINA_TRUE);
  utc_check_eq(result, EINA_TRUE);
}

/**
* @brief checking if returns TRUE when initiated with a correct webview and
* enabled draws transparent background set to FALSE.
*/
TEST_F(utc_blink_ewk_view_draws_transparent_background_set, POS_TEST2)
{
  Eina_Bool result = ewk_view_draws_transparent_background_set(GetEwkWebView(), EINA_FALSE);
  utc_check_eq(result, EINA_TRUE);
}

/**
* @brief checking if returns FALSE when initiated with NULL webview.
*/
TEST_F(utc_blink_ewk_view_draws_transparent_background_set, NEG_TEST)
{
  Eina_Bool result = ewk_view_draws_transparent_background_set(NULL, EINA_FALSE);
  utc_check_eq(result, EINA_FALSE);
}
