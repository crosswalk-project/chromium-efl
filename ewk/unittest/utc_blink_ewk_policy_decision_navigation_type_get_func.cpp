// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_policy_decision_navigation_type_get : public utc_blink_ewk_base {
protected:

  void PostSetUp()
  {
    evas_object_smart_callback_add(GetEwkWebView(), "policy,navigation,decide", policy_navigation_decide, this);
  }

  void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(), "policy,navigation,decide", policy_navigation_decide);
  }

  void LoadFinished(Evas_Object* webview) {

    EventLoopStop(utc_blink_ewk_base::Failure);
  }

  static void policy_navigation_decide(void* data, Evas_Object* webview, void* event_info)
  {
    utc_message("[Policy , Decision] :: \n");
    utc_blink_ewk_policy_decision_navigation_type_get *owner = static_cast<utc_blink_ewk_policy_decision_navigation_type_get*>(data);

    Ewk_Policy_Decision* policy = (Ewk_Policy_Decision*)event_info;

    if(policy && (
        ewk_policy_decision_navigation_type_get(policy) == EWK_POLICY_NAVIGATION_TYPE_LINK_CLICKED ||
        ewk_policy_decision_navigation_type_get(policy) == EWK_POLICY_NAVIGATION_TYPE_FORM_SUBMITTED ||
        ewk_policy_decision_navigation_type_get(policy) == EWK_POLICY_NAVIGATION_TYPE_BACK_FORWARD ||
        ewk_policy_decision_navigation_type_get(policy) == EWK_POLICY_NAVIGATION_TYPE_RELOAD  ||
        ewk_policy_decision_navigation_type_get(policy) == EWK_POLICY_NAVIGATION_TYPE_FORM_RESUBMITTED ||
        ewk_policy_decision_navigation_type_get(policy) == EWK_POLICY_NAVIGATION_TYPE_OTHER))

      owner->EventLoopStop(utc_blink_ewk_base::Success);
  }
};

/**
 * @brief Tests if ewk_policy_decision_navigation_type_get returns the correct navigation type
 */
TEST_F(utc_blink_ewk_policy_decision_navigation_type_get, POS_TEST)
{
  Eina_Bool result = ewk_view_url_set(GetEwkWebView(), "http://www.google.com");
  if (!result)
    FAIL();

  utc_blink_ewk_base::MainLoopResult main_result = EventLoopStart();
  if (main_result != utc_blink_ewk_base::Success)
    FAIL();
}

/**
 * @brief Tests if ewk_policy_decision_navigation_type_get return EWK_POLICY_NAVIGATION_TYPE_OTHER incase policy decision is NULL
 */
TEST_F(utc_blink_ewk_policy_decision_navigation_type_get, NEG_TEST)
{
  utc_check_eq(ewk_policy_decision_navigation_type_get(NULL), EWK_POLICY_NAVIGATION_TYPE_OTHER);
}
