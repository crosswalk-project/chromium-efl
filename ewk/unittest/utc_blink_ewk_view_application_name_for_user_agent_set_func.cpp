// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_view_application_name_for_user_agent_set : public utc_blink_ewk_base
{
};

/**
 * @brief Positive test case of ewk_view_user_agent_set()
 */
TEST_F(utc_blink_ewk_view_application_name_for_user_agent_set, POS_TEST)
{
  const char* testString = "TEST_STRING";
  ASSERT_EQ(EINA_TRUE, ewk_view_application_name_for_user_agent_set(GetEwkWebView(), testString));
  const char* userAgent = ewk_view_application_name_for_user_agent_get(GetEwkWebView());
  EXPECT_EQ(strcmp(userAgent, testString), 0);
}

/**
 * @brief Negative test case of ewk_view_user_agent_set()
 */
TEST_F(utc_blink_ewk_view_application_name_for_user_agent_set, NEG_TEST)
{
  EXPECT_EQ(EINA_FALSE, ewk_view_application_name_for_user_agent_set(GetEwkWebView(), ""));
}
