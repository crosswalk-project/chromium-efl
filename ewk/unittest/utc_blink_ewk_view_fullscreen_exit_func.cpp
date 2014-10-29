// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_view_fullscreen_exit : public utc_blink_ewk_base
{
};

/**
 * @brief Checking whether the custom header is added properly.
 */
TEST_F(utc_blink_ewk_view_fullscreen_exit, POS_TEST)
{
  ewk_view_fullscreen_exit(GetEwkWebView());
  utc_check_true(true);
}

TEST_F(utc_blink_ewk_view_fullscreen_exit, NEG_TEST)
{
  ewk_view_fullscreen_exit(NULL);
  utc_check_true(true);
}
