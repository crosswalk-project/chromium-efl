// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_policy_decision_response_mime_get : public utc_blink_ewk_base {
protected:

  void PostSetUp()
  {
    evas_object_smart_callback_add(GetEwkWebView(), "policy,response,decide", (void(*)(void*, Evas_Object*, void*))policy_response_decide, this);
  }

  void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(), "policy,response,decide", (void(*)(void*, Evas_Object*, void*))policy_response_decide);
  }

  void LoadFinished(Evas_Object* webview)
  {
    EventLoopStop(utc_blink_ewk_base::Failure);
  }

  static void policy_response_decide(utc_blink_ewk_policy_decision_response_mime_get* owner, Evas_Object* webview, Ewk_Policy_Decision* policy_decision)
  {
    utc_message("[policy_response_decide] :: \n");
    ASSERT_TRUE(NULL != owner);

    if (policy_decision && ewk_policy_decision_response_mime_get(policy_decision)) {
      owner->EventLoopStop(Success);
    }
  }
};

/**
 * @brief Tests if the mimetype for policy decision is returned properly.
 */
TEST_F(utc_blink_ewk_policy_decision_response_mime_get, POS_TEST)
{
  ASSERT_TRUE(ewk_view_url_set(GetEwkWebView(), "http://www.google.com"));
  ASSERT_EQ(Success, EventLoopStart());
}

/**
 * @brief Tests if function works properly in case of NULL of a webview
 */
TEST_F(utc_blink_ewk_policy_decision_response_mime_get, NEG_TEST)
{
  ASSERT_EQ(0, ewk_policy_decision_response_mime_get(0));
}
