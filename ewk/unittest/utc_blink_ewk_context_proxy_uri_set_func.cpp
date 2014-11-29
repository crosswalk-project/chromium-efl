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

class utc_blink_ewk_context_proxy_uri_set : public utc_blink_ewk_base
{
protected:
  static const char*const url;
};

const char*const utc_blink_ewk_context_proxy_uri_set::url="http://proxy.tc.url";

/**
 * @brief Positive TC for ewk_context_proxy_uri_set()
 */
TEST_F(utc_blink_ewk_context_proxy_uri_set, POS_TEST)
{
  /* TODO: this code should use ewk_context_proxy_uri_set and check its behaviour.
  Results should be reported using one of utc_ macros */
  Ewk_Context* defaultContext = ewk_context_default_get();
  if (!defaultContext)
    utc_fail();
  ewk_context_proxy_uri_set(defaultContext, url);
  utc_check_str_eq(ewk_context_proxy_uri_get(defaultContext), url);
}

/**
 * @brief Negative TC for ewk_context_proxy_uri_set()
 */
TEST_F(utc_blink_ewk_context_proxy_uri_set, NEG_TEST)
{
  /* TODO: this code should use ewk_context_proxy_uri_set and check its behaviour.
  Results should be reported using one of utc_ macros */
  Ewk_Context* defaultContext = ewk_context_default_get();
  if (!defaultContext)
    utc_fail();
  ewk_context_proxy_uri_set(NULL, NULL);
  utc_pass();
}
