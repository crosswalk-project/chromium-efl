// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_view_text_find_highlight_clear : public utc_blink_ewk_base {
};

/**
 * @brief checking if returns EINA_TRUE when initiated with a correct webview
 */
TEST_F(utc_blink_ewk_view_text_find_highlight_clear, POS_TEST)
{
  Eina_Bool result = ewk_view_text_find_highlight_clear(GetEwkWebView());
  if (!result) {
    FAIL();
  }
  EXPECT_EQ(result, EINA_TRUE);
}

/**
 * @brief checking if returns EINA_FALSE when initiated with a NULL webview
 */
TEST_F(utc_blink_ewk_view_text_find_highlight_clear, NEG_TEST)
{
  Eina_Bool result = ewk_view_text_find_highlight_clear(NULL);
  EXPECT_EQ(result, EINA_FALSE);
}

