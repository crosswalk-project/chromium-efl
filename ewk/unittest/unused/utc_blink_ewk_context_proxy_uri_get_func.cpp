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

/* Define those macros _before_ you include the utc_blink_ewk.h header file. */
#define TESTED_FUN_NAME ewk_context_proxy_uri_get
#define POSITIVE_TEST_FUN_NUM 1
#define NEGATIVE_TEST_FUN_NUM 1

#include "utc_blink_ewk.h"

#define URL "http://proxy.tc.url"

extern struct Ewk_Test_View test_view;

class utc_blink_ewk_context_proxy_uri_get : public ::testing::Test
{
 protected:
  utc_blink_ewk_context_proxy_uri_get()
  {
  }

  ~utc_blink_ewk_context_proxy_uri_get()
  {
  }

  /* Startup function */
  void SetUp()
  {
    utc_blink_ewk_test_init();
  }

  /* Cleanup function */
  void TearDown()
  {
    utc_blink_ewk_test_end();
  }
};

/**
 * @brief Positive TC for ewk_context_proxy_uri_get()
 */
TEST_F(utc_blink_ewk_context_proxy_uri_get, POS_TEST)
{
  /* TODO: this code should use ewk_context_proxy_uri_get and check its behaviour.
  Results should be reported using one of utc_ macros */
  Ewk_Context* defaultContext = ewk_context_default_get();
  if (!defaultContext)
    utc_fail();
  ewk_context_proxy_uri_set(defaultContext, URL);
  utc_check_str_eq(ewk_context_proxy_uri_get(defaultContext), URL);
}

/**
 * @brief Negative TC for ewk_context_proxy_uri_get()
 */
TEST_F(utc_blink_ewk_context_proxy_uri_get, NEG_TEST)
{
  /* TODO: this code should use ewk_context_proxy_uri_get and check its behaviour.
  Results should be reported using one of utc_ macros */
  Ewk_Context* defaultContext = ewk_context_default_get();
  if (!defaultContext)
    utc_fail();
  ewk_context_proxy_uri_set(defaultContext, URL);
  if (ewk_context_proxy_uri_get(0))
    utc_fail();
  utc_pass();
}
