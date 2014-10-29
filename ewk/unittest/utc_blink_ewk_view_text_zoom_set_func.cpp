// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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

