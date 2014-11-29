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

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_view_text_find_highlight_clear : public utc_blink_ewk_base {
};

/**
 * @brief checking if returns EINA_TRUE when initiated with a correct webview
 */
TEST_F(utc_blink_ewk_view_text_find_highlight_clear, POS_TEST)
{
  Eina_Bool result = ewk_view_text_find_highlight_clear(GetEwkWebView());
  if (!result) {
    FAIL();
  }
  EXPECT_EQ(result, EINA_TRUE);
}

/**
 * @brief checking if returns EINA_FALSE when initiated with a NULL webview
 */
TEST_F(utc_blink_ewk_view_text_find_highlight_clear, NEG_TEST)
{
  Eina_Bool result = ewk_view_text_find_highlight_clear(NULL);
  EXPECT_EQ(result, EINA_FALSE);
}

