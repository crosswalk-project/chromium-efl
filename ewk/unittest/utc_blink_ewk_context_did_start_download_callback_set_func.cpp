/*
 * chromium EFL
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

class utc_blink_ewk_context_did_start_download_callback_set : public utc_blink_ewk_base
{
protected:
  void load_finished(Evas_Object* webview)
  {
    EventLoopStop(Success);
  }

  static void policy_navigation_decide(void* data, Evas_Object* webview, void* event_info)
  {
    utc_message("[policy_navigation_decide] ::");

    Ewk_Policy_Decision* policy_decision = static_cast<Ewk_Policy_Decision*>(event_info);

    if (!ewk_policy_decision_download(policy_decision) && data)
        static_cast<utc_blink_ewk_context_did_start_download_callback_set*>(data)->EventLoopStop(Failure);
  }

  static void did_start_download_cb(const char* download_url, void* user_data)
  {
    utc_message("[did_start_download_cb] ::");

    if(user_data)
      static_cast<utc_blink_ewk_context_did_start_download_callback_set*>(user_data)->EventLoopStop( download_url?Success:Failure );
  }

  /* Startup function */
  void PostSetUp()
  {
    evas_object_smart_callback_add( GetEwkWebView(), "policy,navigation,decide", policy_navigation_decide, this);
  }

  /* Cleanup function */
  void PreTearDown()
  {
    evas_object_smart_callback_del( GetEwkWebView(), "policy,navigation,decide", policy_navigation_decide);
  }
};

/**
 * @brief Checking whether callback function for started download is called.
 */
TEST_F(utc_blink_ewk_context_did_start_download_callback_set, POS_TEST)
{
  ewk_context_did_start_download_callback_set(ewk_view_context_get(GetEwkWebView()), did_start_download_cb, this);

  if (!ewk_view_url_set(GetEwkWebView(), "http://www.google.com")) {
    utc_message("[ewk_view_url_set error] :: \n");
    utc_fail();
  }
}

/**
 * @brief Checking whether function works properly in case of NULL of a context.
 */
TEST_F(utc_blink_ewk_context_did_start_download_callback_set, NEG_TEST1)
{
  ewk_context_did_start_download_callback_set(NULL, did_start_download_cb, this);
  utc_pass();
}

/**
 * @brief Checking whether function works properly in case of NULL of a callback for started download.
 */
TEST_F(utc_blink_ewk_context_did_start_download_callback_set, NEG_TEST2)
{
  ewk_context_did_start_download_callback_set(ewk_view_context_get(GetEwkWebView()), NULL, this);
  utc_pass();
}
