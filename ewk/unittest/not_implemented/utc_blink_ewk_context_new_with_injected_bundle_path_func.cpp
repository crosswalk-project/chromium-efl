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
#define TESTED_FUN_NAME ewk_context_new_with_injected_bundle_path
#define POSITIVE_TEST_FUN_NUM 1
#define NEGATIVE_TEST_FUN_NUM 1

#include "utc_blink_ewk.h"

extern struct Ewk_Test_View test_view;

class utc_blink_ewk_context_new_with_injected_bundle_path : public ::testing::Test
{
 protected:
  utc_blink_ewk_context_new_with_injected_bundle_path()
  {
  }

  ~utc_blink_ewk_context_new_with_injected_bundle_path()
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
 * @brief Checkeing whether a new Ewk_Context is created by a path of injected bundle library.
          Depricated API
 */
TEST_F(utc_blink_ewk_context_new_with_injected_bundle_path, POS_TEST)
{
  Eina_Bool result = EINA_FALSE;
  char* full_path = generate_full_resource_path("/ewk_context/injected_bundle/bundle-sample.so");
  if (!full_path)
    utc_fail();

  Ewk_Context* context = ewk_context_new_with_injected_bundle_path(full_path);
  free(full_path);
  if (!context)
    result = EINA_TRUE;

  utc_check_eq(result, EINA_TRUE);
}

/**
 * @brief Checking whether function works properly in case of NULL of a path of injected bundle library.
 */
TEST_F(utc_blink_ewk_context_new_with_injected_bundle_path, NEG_TEST)
{
  Ewk_Context* context = ewk_context_new_with_injected_bundle_path(NULL);
  if (context)
    utc_fail();
}
