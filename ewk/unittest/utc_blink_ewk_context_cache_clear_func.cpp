// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_context_cache_clear : public utc_blink_ewk_base
{
};

/**
 * @brief Checking whether cache is cleared.
 */
TEST_F(utc_blink_ewk_context_cache_clear, POS_TEST)
{
  Eina_Bool result = ewk_context_cache_clear(ewk_context_default_get());
  if (!result) {
    FAIL();
  }
  EXPECT_EQ(result, EINA_TRUE);
}

/**
 * @brief Checking whether function works properly in case of NULL of a context.
 */
TEST_F(utc_blink_ewk_context_cache_clear, NEG_TEST)
{
  Eina_Bool result = ewk_context_cache_clear(NULL);
  if (result) {
    FAIL();
  }
  EXPECT_NE(result, EINA_TRUE);
}
