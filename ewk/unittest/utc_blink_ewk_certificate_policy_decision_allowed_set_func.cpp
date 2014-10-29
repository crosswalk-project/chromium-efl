// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/* Define those macros _before_ you include the utc_blink_ewk.h header file. */

#include "utc_blink_ewk_base.h"

#define URL "https://www.pcwebshop.co.uk"

class utc_blink_ewk_certificate_policy_decision_allowed_set  : public utc_blink_ewk_base
{
protected:

  void PostSetUp()
  {
    evas_object_smart_callback_add(GetEwkWebView(), "request,certificate,confirm", policy_decision, this);
  }

  void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(), "request,certificate,confirm", policy_decision);
  }

  void LoadFinished(Evas_Object* webview)
  {
    EventLoopStop(utc_blink_ewk_base::Failure); // will noop if EventLoopStop was alraedy called
  }

  static void policy_decision(void* data, Evas_Object* webview, void* event_info)
  {
    utc_message("[policy decision] :: \n");
    utc_blink_ewk_certificate_policy_decision_allowed_set *owner = static_cast<utc_blink_ewk_certificate_policy_decision_allowed_set*>(data);

    Ewk_Certificate_Policy_Decision* policy = (Ewk_Certificate_Policy_Decision*)event_info;

    if (policy) {
      ewk_certificate_policy_decision_allowed_set(policy, true);
      //TODO we need to check here correctness of the API action
      owner->EventLoopStop(utc_blink_ewk_base::Success);
      return;
    }
  }
};

/**
* @brief Checking whether sending cancellation notification for authentication challenge works properly.
*/
TEST_F(utc_blink_ewk_certificate_policy_decision_allowed_set, POS_TEST)
{
  Eina_Bool result = ewk_view_url_set(GetEwkWebView(), URL);

  if (!result)
    FAIL();

  utc_blink_ewk_base::MainLoopResult main_result = EventLoopStart();

  if (main_result != utc_blink_ewk_base::Success)
    FAIL();

  evas_object_show(GetEwkWebView());
  evas_object_show(GetEwkWindow());
}

/**
* @brief Checking whether function works properly in case of NULL of a webview.
*/
/*
//TODO : need to implement properly
TEST_F(utc_blink_ewk_certificate_policy_decision_allowed_set, NEG_TEST)
{
  is_failed = EINA_FALSE;
  is_Accepted = EINA_FALSE;

  Eina_Bool result = ewk_view_url_set(NULL, URL);
  if (result)
    utc_fail();

  result = is_Accepted;

  evas_object_show(test_view.webview);
  evas_object_show(test_view.window);
  utc_check_ne(result, EINA_TRUE);
}
*/
