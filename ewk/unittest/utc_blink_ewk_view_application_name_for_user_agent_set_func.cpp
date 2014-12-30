// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_view_application_name_for_user_agent_set : public utc_blink_ewk_base
{
};

/**
 * @brief Negative test case of ewk_view_application_name_for_user_agent_set
 */
TEST_F(utc_blink_ewk_view_application_name_for_user_agent_set, NullArg)
{
  EXPECT_EQ(EINA_FALSE, ewk_view_application_name_for_user_agent_set(NULL, ""));
  EXPECT_EQ(EINA_FALSE, ewk_view_application_name_for_user_agent_set(GetEwkWebView(), NULL));
}

/**
 * @brief Positive test case of ewk_view_application_name_for_user_agent_set
 */
TEST_F(utc_blink_ewk_view_application_name_for_user_agent_set, POS_TEST)
{
  ASSERT_EQ(EINA_TRUE, ewk_view_application_name_for_user_agent_set(GetEwkWebView(), "TEST"));
  EXPECT_STREQ("TEST", ewk_view_application_name_for_user_agent_get(GetEwkWebView()));
}

TEST_F(utc_blink_ewk_view_application_name_for_user_agent_set, EmptyString)
{
  EXPECT_EQ(EINA_TRUE, ewk_view_application_name_for_user_agent_set(GetEwkWebView(), ""));
  // if set empty string (not null) than assign PRODUCT_NAME (version_info_efl.h)
  EXPECT_STREQ("efl-webengine", ewk_view_application_name_for_user_agent_get(GetEwkWebView()));
}
