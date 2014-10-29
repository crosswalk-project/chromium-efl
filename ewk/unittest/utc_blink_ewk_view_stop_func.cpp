// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_view_stop : public utc_blink_ewk_base
{
protected:

  static const char* const sample_html_file;
};

const char* const utc_blink_ewk_view_stop::sample_html_file = "/common/sample.html";

/**
 * @brief Positive  test case of ewk_view_stop()
 * ewk_view_stop should return TRUE  as we are stopping the load with a valid url
 */
TEST_F(utc_blink_ewk_view_stop, POS_TEST)
{
  if (!ewk_view_url_set(GetEwkWebView(), GetResourceUrl(sample_html_file).c_str()))
    FAIL();

  Eina_Bool result = ewk_view_stop(GetEwkWebView());
  EXPECT_EQ(result, EINA_TRUE);
}

/**
 * @brief Negative  test case of ewk_view_stop()
 * ewk_view_stop should return FALSE as we are stopping the load with NULL
 */
TEST_F(utc_blink_ewk_view_stop, NEG_TEST)
{
  Eina_Bool result = ewk_view_stop(NULL);
  EXPECT_EQ(result, EINA_FALSE);
}
