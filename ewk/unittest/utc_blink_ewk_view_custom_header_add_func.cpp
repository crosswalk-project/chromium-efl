// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_view_custom_header_add : public utc_blink_ewk_base
{
};

/**
 * @brief Checking whether the custom header is added properly.
 */
TEST_F(utc_blink_ewk_view_custom_header_add, POS_TEST)
{
  EXPECT_EQ(EINA_TRUE, ewk_view_custom_header_add(GetEwkWebView(), "Test-Name", "Test-Value"));
}

/**
 * @brief Checking whether function works properly in case of NULL of a webview.
 */
TEST_F(utc_blink_ewk_view_custom_header_add, NEG_TEST)
{
  EXPECT_EQ(EINA_FALSE, ewk_view_custom_header_add(NULL, "Test-Name", "Test-Value"));
}
