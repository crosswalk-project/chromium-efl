/*
 * chromium EFL
 *
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

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
