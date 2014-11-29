/*
 * chromium EFL
 *
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

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
