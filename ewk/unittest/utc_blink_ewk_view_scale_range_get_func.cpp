// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_view_scale_range_get : public utc_blink_ewk_base
{
};

/**
  * @brief Positive test case of ewk_view_scale_range_get().
  */
TEST_F(utc_blink_ewk_view_scale_range_get, POS_TEST)
{
  double minimumScale = 0;
  double maximumScale = 0;
  Eina_Bool result = EINA_TRUE;

  ewk_view_scale_range_get(GetEwkWebView(), &minimumScale, &maximumScale);

  if ((minimumScale == 0) || (maximumScale == 0))
    result = EINA_FALSE;

  utc_check_eq(result, EINA_TRUE);
}

/**
  * @brief Checking whether function works properly in case of NULL of a webview.
  */
TEST_F(utc_blink_ewk_view_scale_range_get, NEG_TEST)
{
  double minimumScale = -1;
  double maximumScale = -1;
  Eina_Bool result = EINA_FALSE;

  ewk_view_scale_range_get(NULL, &minimumScale, &maximumScale);

  if ((minimumScale == -1) && (maximumScale == -1))
    result = EINA_TRUE;

  utc_check_eq(result, EINA_TRUE);
}
