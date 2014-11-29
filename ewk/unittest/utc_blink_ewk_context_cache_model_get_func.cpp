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

class utc_blink_ewk_context_cache_model_get : public utc_blink_ewk_base
{
};

/**
 * @brief Check if cache model is correctly retrieved as EWK_CACHE_MODEL_DOCUMENT_VIEWER.
 */
TEST_F(utc_blink_ewk_context_cache_model_get, POS_TEST1)
{
  Ewk_Cache_Model orig_model = EWK_CACHE_MODEL_DOCUMENT_VIEWER;
  // set cache model
  Eina_Bool result = ewk_context_cache_model_set(ewk_context_default_get(), orig_model);
  ASSERT_EQ(EINA_TRUE, result);

  // check if returned model is the same as original model
  Ewk_Cache_Model new_model = ewk_context_cache_model_get(ewk_context_default_get());
  ASSERT_EQ(orig_model, new_model);
}

/**
 * @brief Check if cache model is correctly retrieved as EWK_CACHE_MODEL_DOCUMENT_BROWSER.
 */
TEST_F(utc_blink_ewk_context_cache_model_get, POS_TEST2)
{
  Ewk_Cache_Model orig_model = EWK_CACHE_MODEL_DOCUMENT_BROWSER;
  // set cache model
  Eina_Bool result = ewk_context_cache_model_set(ewk_context_default_get(), orig_model);
  ASSERT_EQ(EINA_TRUE, result);

  // check if returned model is the same as original model
  Ewk_Cache_Model new_model = ewk_context_cache_model_get(ewk_context_default_get());
  ASSERT_EQ(orig_model, new_model);
}

/**
 * @brief Check if cache model is correctly retrieved as EWK_CACHE_MODEL_PRIMARY_WEBBROWSER.
 */
TEST_F(utc_blink_ewk_context_cache_model_get, POS_TEST3)
{
  Ewk_Cache_Model orig_model = EWK_CACHE_MODEL_PRIMARY_WEBBROWSER;
  // set cache model
  Eina_Bool result = ewk_context_cache_model_set(ewk_context_default_get(), orig_model);
  ASSERT_EQ(EINA_TRUE, result);

  // check if returned model is the same as original model
  Ewk_Cache_Model new_model = ewk_context_cache_model_get(ewk_context_default_get());
  ASSERT_EQ(orig_model, new_model);
}

/**
 * @brief Check if function works properly in case of a NULL context.
 */
TEST_F(utc_blink_ewk_context_cache_model_get, NEG_TEST1)
{
  Ewk_Cache_Model model = ewk_context_cache_model_get(NULL);
  ASSERT_TRUE(model == EWK_CACHE_MODEL_DOCUMENT_VIEWER);
}
