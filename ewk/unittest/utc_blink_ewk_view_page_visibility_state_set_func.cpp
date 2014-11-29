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

class utc_blink_ewk_view_page_visibility_state_set : public utc_blink_ewk_base
{
};

TEST_F(utc_blink_ewk_view_page_visibility_state_set, POS_TEST1)
{
  Ewk_Page_Visibility_State test_state = EWK_PAGE_VISIBILITY_STATE_HIDDEN;
  Eina_Bool result = ewk_view_page_visibility_state_set(GetEwkWebView(), test_state, EINA_TRUE);
  utc_check_eq(result, EINA_TRUE);
}

TEST_F(utc_blink_ewk_view_page_visibility_state_set, POS_TEST2)
{
  Ewk_Page_Visibility_State test_state = EWK_PAGE_VISIBILITY_STATE_VISIBLE;
  Eina_Bool result = ewk_view_page_visibility_state_set(GetEwkWebView(), test_state, EINA_FALSE);
  utc_check_eq(result, EINA_TRUE);
}

TEST_F(utc_blink_ewk_view_page_visibility_state_set, NEG_TEST)
{
  Ewk_Page_Visibility_State test_state = EWK_PAGE_VISIBILITY_STATE_HIDDEN;
  Eina_Bool result = ewk_view_page_visibility_state_set(NULL, test_state, EINA_FALSE);
  utc_check_eq(result, EINA_FALSE);
}
