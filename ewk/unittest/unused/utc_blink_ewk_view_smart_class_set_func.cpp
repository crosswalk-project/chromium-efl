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
#define TESTED_FUN_NAME ewk_view_smart_class_set
#define POSITIVE_TEST_FUN_NUM 1
#define NEGATIVE_TEST_FUN_NUM 1

#include "utc_blink_ewk.h"

extern struct Ewk_Test_View test_view;

class utc_blink_ewk_view_smart_class_set : public ::testing::Test
{
 protected:
  utc_blink_ewk_view_smart_class_set()
  {
  }

  ~utc_blink_ewk_view_smart_class_set()
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
 * @brief Checking whether the smart class is initialized properly.
 */
TEST_F(utc_blink_ewk_view_smart_class_set, POS_TEST)
{
  static Ewk_View_Smart_Class ewk_smart_class = EWK_VIEW_SMART_CLASS_INIT_NAME_VERSION("tcblink");
  Eina_Bool result = ewk_view_smart_class_set(&ewk_smart_class);
  utc_check_eq(result, EINA_TRUE);
}

/**
 * @brief Checking whether function works properly in case of NULL of a smart class object.
 */
TEST_F(utc_blink_ewk_view_smart_class_set, NEG_TEST)
{
  Eina_Bool result = ewk_view_smart_class_set(NULL);
  utc_check_ne(result, EINA_TRUE);
}
