// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_view_html_string_load : public utc_blink_ewk_base
{
  void LoadFinished(Evas_Object*)
  {
    EventLoopStop(Success);
  }
};

/**
 * @brief Negative test case of ewk_view_html_string_load()
 */

TEST_F(utc_blink_ewk_view_html_string_load, NullAsView)
{
  ASSERT_EQ(EINA_FALSE, ewk_view_html_string_load(NULL, "<html></html>", NULL, NULL));
}

TEST_F(utc_blink_ewk_view_html_string_load, NullAsHtml)
{
  ASSERT_EQ(EINA_FALSE, ewk_view_html_string_load(GetEwkWebView(), NULL, NULL, NULL));
}

/**
 * @brief Positive test case of ewk_view_html_string_load()
 */
TEST_F(utc_blink_ewk_view_html_string_load, SimplePage)
{
  ASSERT_EQ(EINA_TRUE, ewk_view_html_string_load(GetEwkWebView(),
            "<html><head><title>ewk_view_html_string_load</title></head></html>", NULL, NULL));
  ASSERT_EQ(Success, EventLoopStart());
  ASSERT_STREQ("ewk_view_html_string_load", ewk_view_title_get(GetEwkWebView()));
}

/**
 * @brief Positive test case of ewk_view_html_string_load()
 */
TEST_F(utc_blink_ewk_view_html_string_load, BaseUri)
{
  ASSERT_EQ(EINA_TRUE, ewk_view_html_string_load(GetEwkWebView(),
            "<html><body onload='document.title=document.URL'></body></html>",
            "http://www.samsung.com/pl/home/", NULL));
  ASSERT_EQ(Success, EventLoopStart());
  ASSERT_STREQ("http://www.samsung.com/pl/home/", ewk_view_title_get(GetEwkWebView()));
}

/**
 * @brief Positive test case of ewk_view_html_string_load()
 */
TEST_F(utc_blink_ewk_view_html_string_load, UnreachableUri)
{
  ASSERT_EQ(EINA_TRUE, ewk_view_html_string_load(GetEwkWebView(), "<html><body></body></html>",
                                                 NULL, "http://www.samsung.com/pl/home/"));
  ASSERT_EQ(Success, EventLoopStart());
  ASSERT_STREQ("http://www.samsung.com/pl/home/", ewk_view_url_get(GetEwkWebView()));
}
