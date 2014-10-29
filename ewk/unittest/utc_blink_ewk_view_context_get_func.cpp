// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_view_context_get : public utc_blink_ewk_base
{
};

/**
 * @brief Tests if returns correct context when initiated with a valid webview.
 */
TEST_F(utc_blink_ewk_view_context_get, POS_TEST)
{
  Ewk_Context* context = ewk_view_context_get(GetEwkWebView());
  if (!context)
    utc_fail();
}

/**
 * @brief Tests if returns NULL context when initiated with a NULL webview.
 */
TEST_F(utc_blink_ewk_view_context_get, NEG_TEST)
{
  Ewk_Context* context = ewk_view_context_get(NULL);
  if (context)
    utc_fail();
}
