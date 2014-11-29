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

class utc_blink_ewk_policy_decision_type_get : public utc_blink_ewk_base {
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
    utc_message("[policy_navigation_decide] :: \n");
    utc_blink_ewk_policy_decision_type_get *owner = static_cast<utc_blink_ewk_policy_decision_type_get*>(data);
    Ewk_Policy_Decision* policy_decision = (Ewk_Policy_Decision*)event_info;

    if( policy_decision && (
      ewk_policy_decision_type_get(policy_decision) == EWK_POLICY_DECISION_USE ||
      ewk_policy_decision_type_get(policy_decision) == EWK_POLICY_DECISION_DOWNLOAD ||
      ewk_policy_decision_type_get(policy_decision) == EWK_POLICY_DECISION_IGNORE))

      owner->EventLoopStop(utc_blink_ewk_base::Success);
  }
};

/**
 * @brief Tests if the host for policy decision is returned properly.
 */
TEST_F(utc_blink_ewk_policy_decision_type_get, POS_TEST)
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
TEST_F(utc_blink_ewk_policy_decision_type_get, NEG_TEST)
{
  utc_check_eq(ewk_policy_decision_type_get(0), EWK_POLICY_DECISION_USE);
}
