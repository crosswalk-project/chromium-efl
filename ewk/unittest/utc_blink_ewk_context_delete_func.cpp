// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_context_delete : public utc_blink_ewk_base
{
};

/**
 * @brief Checkeing whether a Ewk_Context is deleted properly.
 */
TEST_F(utc_blink_ewk_context_delete, POS_TEST1)
{
  Ewk_Context*const context = ewk_context_new();
  EXPECT_TRUE(NULL!=context);
  ewk_context_delete(context);
  // TODO: check context for not exists
}

TEST_F(utc_blink_ewk_context_delete, POS_TEST2)
{
  Ewk_Context*const context = ewk_context_default_get();
  ASSERT_TRUE(context);

  /*
   *ewk_context_default_get does not increase refcount of default context.
   *We need it do manually by ewk_context_ref, because ewk_context_delete
   *decreases it.
   */
  ASSERT_TRUE(NULL!=ewk_context_ref(context));
  ewk_context_delete(context);
  EXPECT_EQ(context, ewk_context_default_get());
}

/**
 * @brief Checking whether function works properly in case of NULL of a context.
 */
TEST_F(utc_blink_ewk_context_delete, NEG_TEST)
{
  ewk_context_delete(NULL);
}
