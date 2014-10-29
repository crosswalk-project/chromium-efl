// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_policy_decision_host_get : public utc_blink_ewk_base
{
 protected:

  void PostSetUp()
  {
    evas_object_smart_callback_add(GetEwkWebView(), "policy,navigation,decide", policy_navigation_decide, this);
  }

  void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(), "policy,navigation,decide", policy_navigation_decide);
  }

  void LoadFinished(Evas_Object* webview)
  {
    EventLoopStop(utc_blink_ewk_base::Failure);
  }

  static void policy_navigation_decide(void* data, Evas_Object* webview, void* event_info)
  {
    utc_message("[policy_navigation_decide] :: \n");
    utc_blink_ewk_policy_decision_host_get *owner = static_cast<utc_blink_ewk_policy_decision_host_get*>(data);
    Ewk_Policy_Decision* policy_decision = (Ewk_Policy_Decision*)event_info;

    if (policy_decision && ewk_policy_decision_host_get(policy_decision)) {

      owner->EventLoopStop(utc_blink_ewk_base::Success);
    }
  }
};

/**
 * @brief Tests if the host for policy decision is returned properly.
 */
TEST_F(utc_blink_ewk_policy_decision_host_get, POS_TEST)
{
  Eina_Bool result = ewk_view_url_set(GetEwkWebView(), "http://www.google.com");
  if (!result)
    FAIL();

  utc_blink_ewk_base::MainLoopResult main_result = EventLoopStart();
  if (main_result != utc_blink_ewk_base::Success)
    FAIL();
}

/**
 * @brief Tests if function works properly in case of NULL of a webview
 */
TEST_F(utc_blink_ewk_policy_decision_host_get, NEG_TEST)
{
  utc_check_eq(NULL, ewk_policy_decision_host_get(0));
}
