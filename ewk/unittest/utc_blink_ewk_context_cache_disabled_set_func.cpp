// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_context_cache_disabled : public utc_blink_ewk_base
{
protected:
};

/**
 * @brief Checking whether cache is cleared.
 */
TEST_F(utc_blink_ewk_context_cache_disabled, POS_TEST1)
{
  Ewk_Context* context = ewk_context_default_get();
  if (!context) {
    FAIL();
  }
  Eina_Bool result = ewk_context_cache_disabled_set(context, true);
  if (!result) {
    FAIL();
  }
  EXPECT_EQ(result, EINA_TRUE);
}

TEST_F(utc_blink_ewk_context_cache_disabled, POS_TEST2)
{
  Eina_Bool result = ewk_context_cache_disabled_set(ewk_context_default_get(), false);
  if (!result) {
    FAIL();
  }
  EXPECT_EQ(result, EINA_TRUE);
}
/**
 * @brief Checking whether function works properly in case of NULL of a context.
 */
TEST_F(utc_blink_ewk_context_cache_disabled, NEG_TEST)
{
  Eina_Bool result = ewk_context_cache_disabled_set(NULL, true);
  if (result) {
    FAIL();
  }
  EXPECT_NE(result, EINA_TRUE);
}
