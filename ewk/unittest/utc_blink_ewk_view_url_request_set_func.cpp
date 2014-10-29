// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <Eina.h>
#include "utc_blink_ewk_base.h"
#define URL "http://google.com"

class utc_blink_ewk_view_url_request_set : public utc_blink_ewk_base
{
protected:
  utc_blink_ewk_view_url_request_set()
    : header(eina_hash_string_small_new(NULL))
  {
    eina_hash_add(header, "Accept", "text/plain");
    eina_hash_add(header, "Referer", "http://samsung.com");
  }

  ~utc_blink_ewk_view_url_request_set() {
    eina_hash_free(header);
  }

protected:
  Eina_Hash *const header;
};

/**
 * @brief Check if set values with correct WebView, Http Method and URL
 */
TEST_F(utc_blink_ewk_view_url_request_set, POS_TEST)
{
  ASSERT_TRUE(ewk_view_url_request_set(GetEwkWebView(), URL, EWK_HTTP_METHOD_GET, header, NULL));
}

/**
 * @brief Check if return false if use correct WebView and Http Method and NULL url
 */
TEST_F(utc_blink_ewk_view_url_request_set, NEG_TEST)
{
  ASSERT_FALSE(ewk_view_url_request_set(GetEwkWebView(), NULL, EWK_HTTP_METHOD_GET, NULL, NULL));
}

/**
 * @brief Check if return false if use NULL url and correct WebView and Http Method
 */
TEST_F(utc_blink_ewk_view_url_request_set, NEG_TEST2)
{
  ASSERT_FALSE(ewk_view_url_request_set(NULL, URL, EWK_HTTP_METHOD_GET, NULL, NULL));
}
