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

class utc_blink_ewk_policy_decision_response_headers_get : public utc_blink_ewk_base
{
protected:

  void PostSetUp()
  {
    evas_object_smart_callback_add(GetEwkWebView(), "policy,response,decide", policy_response_decide, this);
  }

  void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(), "policy,response,decide", policy_response_decide);
  }

  void LoadFinished(Evas_Object* webview) {
    EventLoopStop(utc_blink_ewk_base::Failure);
  }

  static void policy_response_decide(void* data, Evas_Object* webview, void* event_info)
  {
    ASSERT_TRUE(data != NULL);
    ASSERT_TRUE(event_info != NULL);
    utc_message("[policy_response_decide] ::");
    utc_blink_ewk_policy_decision_response_headers_get* const owner = static_cast<utc_blink_ewk_policy_decision_response_headers_get*>(data);
    Ewk_Policy_Decision* const policy_decision = static_cast<Ewk_Policy_Decision*>(event_info);
    const Eina_Hash* const headers = ewk_policy_decision_response_headers_get(policy_decision);
    owner->EventLoopStop((NULL != headers) ? Success : Failure);
  }
};

/**
 * @brief Tests if the headers for policy decision is returned properly.
 */
TEST_F(utc_blink_ewk_policy_decision_response_headers_get, POS_TEST)
{
  ASSERT_TRUE(ewk_view_url_set(GetEwkWebView(), "http://www.samsung.com"));

  EXPECT_EQ(Success, EventLoopStart());
}

/**
 * @brief Tests if function works properly in case of NULL of a webview
 */
TEST_F(utc_blink_ewk_policy_decision_response_headers_get, NEG_TEST)
{
  EXPECT_EQ(0, ewk_policy_decision_response_headers_get(0));
}
