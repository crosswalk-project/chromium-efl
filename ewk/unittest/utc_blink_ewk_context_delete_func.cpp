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
