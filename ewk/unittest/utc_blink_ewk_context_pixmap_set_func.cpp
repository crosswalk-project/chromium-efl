// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_context_pixmap_set : public utc_blink_ewk_base
{
};

/**
 * @brief Tests if can set pixmap id.
 */
TEST_F(utc_blink_ewk_context_pixmap_set, POS_TEST)
{
  Ewk_Context* context = ewk_context_default_get();
  ASSERT_TRUE(context);

  ASSERT_EQ(EINA_TRUE, ewk_context_pixmap_set(context, 1));

  EXPECT_EQ(1, ewk_context_pixmap_get(context));
}

/**
 * @brief Tests if returns false when context is null.
 */
TEST_F(utc_blink_ewk_context_pixmap_set, NEG_TEST)
{
  ASSERT_EQ(EINA_FALSE, ewk_context_pixmap_set(NULL, 1));
}
