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

class utc_blink_ewk_view_context_get : public utc_blink_ewk_base
{
};

/**
 * @brief Tests if returns correct context when initiated with a valid webview.
 */
TEST_F(utc_blink_ewk_view_context_get, POS_TEST)
{
  Ewk_Context* context = ewk_view_context_get(GetEwkWebView());
  if (!context)
    utc_fail();
}

/**
 * @brief Tests if returns NULL context when initiated with a NULL webview.
 */
TEST_F(utc_blink_ewk_view_context_get, NEG_TEST)
{
  Ewk_Context* context = ewk_view_context_get(NULL);
  if (context)
    utc_fail();
}
