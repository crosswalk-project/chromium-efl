// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

#define URL "http://proxy.tc.url"

class utc_blink_ewk_context_proxy_uri_get : public utc_blink_ewk_base
{
public:
    Ewk_Context* defaultContext;
protected:
    void PostSetUp()
    {
        defaultContext = ewk_view_context_get(GetEwkWebView());
        ASSERT_TRUE(defaultContext);
        ewk_context_proxy_uri_set(defaultContext, URL);
    }
};

/**
 * @brief Positive TC for ewk_context_proxy_uri_get()
 */
TEST_F(utc_blink_ewk_context_proxy_uri_get, POS_TEST)
{
  EXPECT_STREQ(ewk_context_proxy_uri_get(defaultContext), URL);
}

/**
 * @brief Negative TC for ewk_context_proxy_uri_get()
 */
TEST_F(utc_blink_ewk_context_proxy_uri_get, NEG_TEST)
{
  EXPECT_STRNE(ewk_context_proxy_uri_get(0), URL);
}
