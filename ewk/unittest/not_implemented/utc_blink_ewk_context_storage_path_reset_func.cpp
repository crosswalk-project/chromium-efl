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
#define TESTED_FUN_NAME ewk_context_storage_path_reset
#define POSITIVE_TEST_FUN_NUM 1
#define NEGATIVE_TEST_FUN_NUM 1

#include "utc_blink_ewk.h"

extern struct Ewk_Test_View test_view;

class utc_blink_ewk_context_storage_path_reset : public ::testing::Test
{
 protected:
  utc_blink_ewk_context_storage_path_reset()
  {
  }

  ~utc_blink_ewk_context_storage_path_reset()
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
 * @brief Tests if resets web storage, web database and application cache paths when initialized with proper arguments.
 */
TEST_F(utc_blink_ewk_context_storage_path_reset, POS_TEST)
{
  Ewk_Context* context = ewk_context_default_get();
  ewk_context_storage_path_reset(context);
  utc_pass();
}

/**
 * @brief Tests if there is no segmentation fault when invoked with NULL argument.
 */
TEST_F(utc_blink_ewk_context_storage_path_reset, NEG_TEST)
{
  ewk_context_storage_path_reset(NULL);
  utc_pass();
}
