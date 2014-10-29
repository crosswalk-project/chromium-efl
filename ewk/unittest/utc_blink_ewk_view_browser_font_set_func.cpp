// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_view_browser_font_set : public utc_blink_ewk_base
{
 protected:
};

/**
* @brief Check whether function works well with correct webview as parameter
*/
TEST_F(utc_blink_ewk_view_browser_font_set, POS_TEST)
{
  ewk_view_browser_font_set(GetEwkWebView());
  SUCCEED();
}

/**
* @brief Checking whether function works properly in case of NULL of a webview.
*/
TEST_F(utc_blink_ewk_view_browser_font_set, NEG_TEST)
{
  ewk_view_browser_font_set(NULL);
  SUCCEED();
}
