/*
 * Chromium EFL
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
