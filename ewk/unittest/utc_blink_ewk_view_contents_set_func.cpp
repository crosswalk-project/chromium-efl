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

class utc_blink_ewk_view_contents_set : public utc_blink_ewk_base
{
};

/**
 * @brief Positive test case of ewk_view_contents_set(),testing results whether contents are set or not.
 */
TEST_F(utc_blink_ewk_view_contents_set, POS_TEST)
{
  Eina_Bool results = ewk_view_contents_set(GetEwkWebView(), "This is Plain Text", 20, 0, 0, 0);
  utc_check_eq(results, EINA_TRUE);
}

/**
 * @brief  checking whether function is working when providing NULL value.
 */
TEST_F(utc_blink_ewk_view_contents_set, NEG_TEST)
{
  utc_check_eq(ewk_view_contents_set(NULL, "This is Plain Text", 0, 0, 0, 0), EINA_FALSE);
}
