// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_context_new_func : public utc_blink_ewk_base
{
};

/**
 * @brief Checkeing whether a Ewk_Context is created properly.
 */
TEST_F(utc_blink_ewk_context_new_func, POS_TEST1)
{
  Ewk_Context* const context = ewk_context_new();
  EXPECT_TRUE(context != NULL);
  ewk_context_delete(context);
}

