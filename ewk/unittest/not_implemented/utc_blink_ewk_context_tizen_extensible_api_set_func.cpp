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
#define TESTED_FUN_NAME ewk_context_tizen_extensible_api_set
#define POSITIVE_TEST_FUN_NUM 1
#define NEGATIVE_TEST_FUN_NUM 1

#include "utc_blink_ewk.h"

extern struct Ewk_Test_View test_view;

class utc_blink_ewk_context_tizen_extensible_api_set : public ::testing::Test
{
 protected:
  utc_blink_ewk_context_tizen_extensible_api_set()
  {
  }

  ~utc_blink_ewk_context_tizen_extensible_api_set()
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
 * @brief Tests if returns TRUE when initialized with a correct webview and
 * each extensible api try to set TRUE.
 */
TEST_F(utc_blink_ewk_context_tizen_extensible_api_set, POS_TEST)
{
  int i;
  Eina_Bool result = EINA_FALSE;
  Ewk_Context* context = ewk_view_context_get(test_view.webview);
  if (!context)
    utc_fail();

  result = ewk_context_tizen_extensible_api_set(context, EWK_EXTENSIBLE_API_BACKGROUND_MUSIC, EINA_TRUE);
  utc_check_ne(result, EINA_TRUE);
}

/**
 * @brief Tests if returns FALSE when initialized with NULL webview.
 */
TEST_F(utc_blink_ewk_context_tizen_extensible_api_set, NEG_TEST)
{
  Eina_Bool result = ewk_context_tizen_extensible_api_set(NULL, EWK_EXTENSIBLE_API_BACKGROUND_MUSIC, EINA_TRUE);
  utc_check_ne(result, EINA_TRUE);
}
