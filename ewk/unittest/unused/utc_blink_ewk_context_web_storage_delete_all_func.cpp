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

/* Define those macros _before_ you include the utc_blink_ewk.h header file. */

#include "utc_blink_ewk.h"

extern struct Ewk_Test_View test_view;

class utc_blink_ewk_context_web_storage_delete_all : public ::testing::Test {
  protected:
    utc_blink_ewk_context_web_storage_delete_all() {
    }

    virtual ~utc_blink_ewk_context_web_storage_delete_all() {
    }

    virtual void SetUp() {
      utc_blink_ewk_test_init();
    }

    virtual void TearDown() {
      utc_blink_ewk_test_end();
    }
  };

/**
* @brief Tests if can delete all web storages
*/
TEST_F(utc_blink_ewk_context_web_storage_delete_all, POS_TEST)
{
  Ewk_Context* context = ewk_view_context_get(test_view.webview);
  if (!ewk_context_web_storage_delete_all(context))
   utc_fail();

  utc_pass();
}


/**
* @brief Tests if returns false when context is null.
*/
TEST_F(utc_blink_ewk_context_web_storage_delete_all, NEG_TEST)
{
  utc_check_eq(ewk_context_web_storage_delete_all(0), EINA_FALSE);
}
