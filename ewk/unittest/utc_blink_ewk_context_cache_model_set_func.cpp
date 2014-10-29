// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
