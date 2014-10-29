// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_view_visibility_set : public utc_blink_ewk_base {
};


TEST_F(utc_blink_ewk_view_visibility_set, POS_TEST1)
{
  Eina_Bool result = ewk_view_visibility_set(GetEwkWebView(), EINA_TRUE);
  EXPECT_EQ(result, EINA_TRUE);
}

TEST_F(utc_blink_ewk_view_visibility_set, POS_TEST2)
{
  Eina_Bool result = ewk_view_visibility_set(GetEwkWebView(), EINA_FALSE);
  EXPECT_EQ(result, EINA_TRUE);
}

TEST_F(utc_blink_ewk_view_visibility_set, NEG_TEST)
{
  Eina_Bool result = ewk_view_visibility_set(NULL, EINA_FALSE);
  EXPECT_EQ(result, EINA_FALSE);
}

