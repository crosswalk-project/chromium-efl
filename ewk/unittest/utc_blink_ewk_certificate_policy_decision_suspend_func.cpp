// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/* Define those macros _before_ you include the utc_blink_ewk.h header file. */

#include "utc_blink_ewk_base.h"

#define URL "https://www.pcwebshop.co.uk"

class utc_blink_ewk_certificate_policy_decision_suspend  : public utc_blink_ewk_base
{
protected:

  void PostSetUp() override
  {
    evas_object_smart_callback_add(GetEwkWebView(), "request,certificate,confirm", policy_decision, this);
  }

  void PreTearDown() override
  {
    evas_object_smart_callback_del(GetEwkWebView(), "request,certificate,confirm", policy_decision);
  }

  void LoadFinished(Evas_Object* webview) override
  {
    utc_message("[policy decision] :: Load finished for URL: %s\n", ewk_view_url_get(webview));

    EventLoopStop(utc_blink_ewk_base::Failure);
  }

  static void policy_decision(void* data, Evas_Object* webview, void* event_info)
  {
    utc_message("[policy decision] :: \n");

    utc_blink_ewk_certificate_policy_decision_suspend *owner =
            static_cast<utc_blink_ewk_certificate_policy_decision_suspend*>(data);
    ASSERT_TRUE(owner) << "Event triggered with invalid data object";

    owner->certificate_policy_decision = static_cast<Ewk_Certificate_Policy_Decision*>(event_info);

    if (owner->certificate_policy_decision && !owner->policy_suspended) {
      ewk_certificate_policy_decision_suspend(owner->certificate_policy_decision);
      owner->policy_suspended = true;
    }
  }

  bool TimeOut() override
  {
    EventLoopStop(policy_suspended ? Success : Failure);

    return true;
  }

protected:
  Ewk_Certificate_Policy_Decision *certificate_policy_decision = nullptr;
  bool policy_suspended = false;
};

/**
* @brief Checking whether certification request confirmation works properly.
*/
TEST_F(utc_blink_ewk_certificate_policy_decision_suspend, POS_TEST)
{
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), URL));

  ASSERT_EQ(Success, EventLoopStart(5));

  ASSERT_TRUE(policy_suspended);
  ASSERT_NE(certificate_policy_decision, nullptr);

  ewk_certificate_policy_decision_allowed_set(certificate_policy_decision, EINA_TRUE);
  ASSERT_EQ(Failure, EventLoopStart(10));
}

/**
* @brief Checking whether function works properly in case of NULL argument.
*/
TEST_F(utc_blink_ewk_certificate_policy_decision_suspend, NEG_TEST)
{
  ASSERT_EQ(EINA_FALSE, ewk_certificate_policy_decision_suspend(NULL));
}
