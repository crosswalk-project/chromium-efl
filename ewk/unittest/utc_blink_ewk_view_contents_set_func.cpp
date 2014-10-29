// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_view_contents_set : public utc_blink_ewk_base
{
};

/**
 * @brief Positive test case of ewk_view_contents_set(),testing results whether contents are set or not.
 */
TEST_F(utc_blink_ewk_view_contents_set, POS_TEST)
{
  Eina_Bool results = ewk_view_contents_set(GetEwkWebView(), "This is Plain Text", 20, 0, 0, 0);
  utc_check_eq(results, EINA_TRUE);
}

/**
 * @brief  checking whether function is working when providing NULL value.
 */
TEST_F(utc_blink_ewk_view_contents_set, NEG_TEST)
{
  utc_check_eq(ewk_view_contents_set(NULL, "This is Plain Text", 0, 0, 0, 0), EINA_FALSE);
}
