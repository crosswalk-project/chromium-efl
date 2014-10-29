// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
