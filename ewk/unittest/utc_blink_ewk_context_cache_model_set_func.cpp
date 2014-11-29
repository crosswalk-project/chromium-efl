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

class utc_blink_ewk_context_cache_model_set : public utc_blink_ewk_base
{
};

/**
 * @brief Checking whether cache model is set to EWK_CACHE_MODEL_DOCUMENT_VIEWER.
 */
TEST_F(utc_blink_ewk_context_cache_model_set, POS_TEST1)
{
  Eina_Bool result = ewk_context_cache_model_set(ewk_context_default_get(), EWK_CACHE_MODEL_DOCUMENT_VIEWER);
  if (!result) {
    FAIL();
  }
  EXPECT_EQ(result, EINA_TRUE);
}

/**
 * @brief Checking whether cache model is set to EWK_CACHE_MODEL_DOCUMENT_BROWSER.
 */
TEST_F(utc_blink_ewk_context_cache_model_set, POS_TEST2)
{
  Eina_Bool result = ewk_context_cache_model_set(ewk_context_default_get(), EWK_CACHE_MODEL_DOCUMENT_BROWSER);
  if (!result) {
    FAIL();
  }
  EXPECT_EQ(result, EINA_TRUE);
}

/**
 * @brief Checking whether cache model is set to EWK_CACHE_MODEL_PRIMARY_WEBBROWSER.
 */
TEST_F(utc_blink_ewk_context_cache_model_set, POS_TEST3)
{
  Eina_Bool result = ewk_context_cache_model_set(ewk_context_default_get(), EWK_CACHE_MODEL_PRIMARY_WEBBROWSER);
  if (!result) {
    FAIL();
  }
  EXPECT_EQ(result, EINA_TRUE);
}

/**
 * @brief Checking whether function works properly in case of NULL of a context.
 */
TEST_F(utc_blink_ewk_context_cache_model_set, NEG_TEST1)
{
  Eina_Bool result = ewk_context_cache_model_set(NULL, EWK_CACHE_MODEL_DOCUMENT_VIEWER);
  if (result) {
    FAIL();
  }
  EXPECT_NE(result, EINA_TRUE);
}

/**
 * @brief Checking whether function works properly in case of NULL of a context.
 */
TEST_F(utc_blink_ewk_context_cache_model_set, NEG_TEST2)
{
  Eina_Bool result = ewk_context_cache_model_set(NULL, EWK_CACHE_MODEL_DOCUMENT_BROWSER);
  if (result) {
    FAIL();
  }
  EXPECT_NE(result, EINA_TRUE);
}

/**
 * @brief Checking whether function works properly in case of NULL of a context.
 */
TEST_F(utc_blink_ewk_context_cache_model_set, NEG_TEST3)
{
  Eina_Bool result = ewk_context_cache_model_set(NULL, EWK_CACHE_MODEL_PRIMARY_WEBBROWSER);
  if (result) {
    FAIL();
  }
  EXPECT_NE(result, EINA_TRUE);
}
