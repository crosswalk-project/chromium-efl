// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
