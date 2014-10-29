// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

#define SITE "@reference.vodafone.com/widgets/testcases.html"
#define CORRECT_PASSWORD "early"
#define CORRECT_URL "http://access:" CORRECT_PASSWORD SITE
#define INCORRECT_PASSWORD ""
#define INCORRECT_URL "http://incorrect:" INCORRECT_PASSWORD SITE

class utc_blink_ewk_policy_decision_password_get : public utc_blink_ewk_base
{
protected:
  utc_blink_ewk_policy_decision_password_get()
    : password(NULL)
  {
  }

  void PostSetUp()
  {
    evas_object_smart_callback_add(GetEwkWebView(), "policy,navigation,decide", policy_navigation_decide, this);
  }

  void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(), "policy,navigation,decide", policy_navigation_decide);
    eina_stringshare_del(password);
  }

  void LoadFinished(Evas_Object* webview)
  {
    EventLoopStop(utc_blink_ewk_base::Failure); // will noop if EventLoopStop was alraedy called
  }

  static void policy_navigation_decide(void* data, Evas_Object* /*webview*/, void* event_info)
  {
    ASSERT_TRUE(NULL != data);
    utc_message("[policy_navigation_decide] :: \n");
    utc_blink_ewk_policy_decision_password_get* owner = NULL;
    OwnerFromVoid(data, &owner);
    Ewk_Policy_Decision* policy_decision = static_cast<Ewk_Policy_Decision*>(event_info);
    owner->password = eina_stringshare_add(ewk_policy_decision_password_get(policy_decision));
    owner->EventLoopStop(policy_decision ? utc_blink_ewk_base::Success : utc_blink_ewk_base::Failure);
  }

protected:
  Eina_Stringshare* password;
};

/**
 * @brief Tests if function works properly in case of
 * correct Ewk_Policy_Decision and correct URL
 */
TEST_F(utc_blink_ewk_policy_decision_password_get, POS_TEST)
{
  ASSERT_TRUE(ewk_view_url_set(GetEwkWebView(), CORRECT_URL));
  ASSERT_EQ(utc_blink_ewk_base::Success, EventLoopStart());
  ASSERT_STREQ(CORRECT_PASSWORD, password);
}

/**
 * @brief Tests if function works properly in case of NULL Ewk_Policy_Decision
 */
TEST_F(utc_blink_ewk_policy_decision_password_get, NEG_TEST)
{
  ASSERT_FALSE(ewk_policy_decision_password_get(0));
}

/**
 * @brief Tests if function works properly in case of
 * correct Ewk_Policy_Decision and incorrect URL
 */
TEST_F(utc_blink_ewk_policy_decision_password_get, NEG_TEST_2)
{
  ASSERT_TRUE(ewk_view_url_set(GetEwkWebView(), INCORRECT_URL));
  ASSERT_EQ(utc_blink_ewk_base::Success, EventLoopStart());
  ASSERT_STREQ(NULL, password);
}
