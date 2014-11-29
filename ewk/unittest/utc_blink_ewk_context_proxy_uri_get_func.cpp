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
