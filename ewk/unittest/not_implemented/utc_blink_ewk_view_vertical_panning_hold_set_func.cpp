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

#include "utc_blink_ewk.h"

#ifndef NULL
#define NULL    0x0
#endif

extern struct Ewk_Test_View test_view;

class utc_blink_ewk_view_vertical_panning_hold_set : public ::testing::Test {
 protected:
  utc_blink_ewk_view_vertical_panning_hold_set() {
  }

  virtual ~utc_blink_ewk_view_vertical_panning_hold_set() {
  }

  /* Startup function */
  virtual void SetUp()
  {
    utc_blink_ewk_test_init();
  }

  /* Cleanup function */
  virtual void TearDown()
  {
    utc_blink_ewk_test_end();
  }
};

/**
* @brief Check whether it is possible to forward when setting panning functionality on or off
*/
TEST_F(utc_blink_ewk_view_vertical_panning_hold_set, POS_TEST)
{
  ewk_view_vertical_panning_hold_set(test_view.webview,EINA_TRUE);
  utc_pass();
}

/**
* @brief Checking whether function works properly in case of NULL of a webview.
*/
TEST_F(utc_blink_ewk_view_vertical_panning_hold_set, NEG_TEST)
{
  ewk_view_vertical_panning_hold_set(NULL,EINA_FALSE);
  utc_pass();
}
