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

#define SITE "reference.vodafone.com/widgets/testcases.html"
#define CORRECT_USER_NAME "access"
#define CORRECT_URL "http://" CORRECT_USER_NAME ":early@" SITE
#define INCORRECT_URL "http://" SITE

class utc_blink_ewk_policy_decision_userid_get : public utc_blink_ewk_base
{
public:
  Eina_Stringshare* user_name;

protected:
  utc_blink_ewk_policy_decision_userid_get()
    : user_name(NULL)
  {
  }

  void PostSetUp()
  {
    evas_object_smart_callback_add(GetEwkWebView(), "policy,navigation,decide", policy_navigation_decide, this);
  }

  void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(), "policy,navigation,decide", policy_navigation_decide);
    eina_stringshare_del(user_name);
  }

  void LoadFinished(Evas_Object* webview)
  {
    EventLoopStop(utc_blink_ewk_base::Failure); // will noop if EventLoopStop was alraedy called
  }

  static void policy_navigation_decide(void* data, Evas_Object* /*webview*/, void* event_info)
  {
    ASSERT_TRUE(NULL != data);
    utc_message("[policy_navigation_decide] :: \n");
    utc_blink_ewk_policy_decision_userid_get* owner = NULL;
    OwnerFromVoid(data, &owner);
    Ewk_Policy_Decision* policy_decision = static_cast<Ewk_Policy_Decision*>(event_info);
    owner->user_name = eina_stringshare_add(ewk_policy_decision_userid_get(policy_decision));
    owner->EventLoopStop(policy_decision ? utc_blink_ewk_base::Success : utc_blink_ewk_base::Failure);
  }
};

/**
 * @brief Tests if function works properly in case of
 * correct Ewk_Policy_Decision and correct URL
 */
TEST_F(utc_blink_ewk_policy_decision_userid_get, POS_TEST)
{
  ASSERT_TRUE(ewk_view_url_set(GetEwkWebView(), CORRECT_URL));
  ASSERT_EQ(utc_blink_ewk_base::Success, EventLoopStart());
  ASSERT_STREQ(CORRECT_USER_NAME, user_name);
}

/**
 * @brief Tests if function works properly in case of NULL Ewk_Policy_Decision
 */
TEST_F(utc_blink_ewk_policy_decision_userid_get, NEG_TEST)
{
    ASSERT_FALSE(ewk_policy_decision_userid_get(0));
}

/**
 * @brief Tests if function works properly in case of
 * correct Ewk_Policy_Decision and incorrect URL
 */
TEST_F(utc_blink_ewk_policy_decision_userid_get, NEG_TEST_2)
{
  ASSERT_TRUE(ewk_view_url_set(GetEwkWebView(), INCORRECT_URL));
  ASSERT_EQ(utc_blink_ewk_base::Success, EventLoopStart());
  ASSERT_STREQ(NULL, user_name);
}
