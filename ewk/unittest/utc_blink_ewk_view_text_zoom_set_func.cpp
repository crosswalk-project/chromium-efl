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

class utc_blink_ewk_view_text_zoom_set : public utc_blink_ewk_base {
};

/**
* @brief Positive test case of ewk_view_text_zoom_set(). Set the value, get it and then compare.
*/
TEST_F(utc_blink_ewk_view_text_zoom_set, POS_TEST1)
{
  double setTextZoomFactor = 4;
  Eina_Bool result = ewk_view_text_zoom_set(GetEwkWebView(), setTextZoomFactor);
  if (!result) {
    FAIL();
  }

  double getTextZoomFactor = ewk_view_text_zoom_get(GetEwkWebView());
  EXPECT_EQ( getTextZoomFactor, setTextZoomFactor);
}

/**
* @brief Checking whether function works properly in case of NULL of a webview.
*/
TEST_F(utc_blink_ewk_view_text_zoom_set, NEG_TEST)
{
  double setTextZoomFactor = 2;
  Eina_Bool result = ewk_view_text_zoom_set(NULL, setTextZoomFactor);
  EXPECT_EQ(result, EINA_FALSE);
}

