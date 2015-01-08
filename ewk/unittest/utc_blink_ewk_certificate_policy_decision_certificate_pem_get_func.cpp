// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/* Define those macros _before_ you include the utc_blink_ewk.h header file. */

#include "utc_blink_ewk_base.h"

#define URL "https://www.pcwebshop.co.uk"

class utc_blink_ewk_certificate_policy_decision_certificate_pem_get : public utc_blink_ewk_base
{
protected:
  void PostSetUp()
  {
    pem = NULL;
    evas_object_smart_callback_add(GetEwkWebView(), "request,certificate,confirm", policy_decision, this);
  }

  void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(), "request,certificate,confirm", policy_decision);
    eina_stringshare_del(pem);
  }

  void LoadFinished(Evas_Object* webview)
  {
    EventLoopStop(utc_blink_ewk_base::Failure); // will noop if EventLoopStop was alraedy called
  }

  static void policy_decision(void* data, Evas_Object* webview, void* event_info)
  {
    utc_message("[policy decision] :: \n");
    utc_blink_ewk_certificate_policy_decision_certificate_pem_get *owner =
        static_cast<utc_blink_ewk_certificate_policy_decision_certificate_pem_get*>(data);

    Ewk_Certificate_Policy_Decision* policy = (Ewk_Certificate_Policy_Decision*)event_info;

    owner->pem = eina_stringshare_add(ewk_certificate_policy_decision_certificate_pem_get(policy));
    if (policy && owner->pem) {
      utc_message("[policy decision] :: pem: %s\n", owner->pem);
      owner->EventLoopStop(utc_blink_ewk_base::Success);
    }
  }

protected:
  Eina_Stringshare* pem;
};

/**
* @brief Checking whether ewk_certificate_policy_decision_certificate_pem_get
* returns not empty string.
*/
TEST_F(utc_blink_ewk_certificate_policy_decision_certificate_pem_get, POS_TEST)
{
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), URL));
  ASSERT_EQ(Success, EventLoopStart());
  ASSERT_STRNE("", pem);
}

/**
* @brief Checking whether function works properly in case of NULL argument.
*/
TEST_F(utc_blink_ewk_certificate_policy_decision_certificate_pem_get, NEG_TEST)
{
  ASSERT_STREQ("", ewk_certificate_policy_decision_certificate_pem_get(NULL));
}
