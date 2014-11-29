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

class utc_blink_ewk_policy_decision_frame_get: public utc_blink_ewk_base
{
protected:
  static void frame_check(void* data, Evas_Object* webview, void* event_info)
  {
    utc_message("  [ %s ]", __PRETTY_FUNCTION__);

    ASSERT_TRUE(data); // test will wait for timeout, nothing we can do here
    utc_blink_ewk_policy_decision_frame_get* owner = NULL;
    OwnerFromVoid(data, &owner);

    if (!event_info) {
      // stop event loop so we won't have to wait for timeout
      owner->EventLoopStop(Failure);
      ASSERT_TRUE(event_info);
    }

    Ewk_Policy_Decision* policy_decision = static_cast<Ewk_Policy_Decision*>(event_info);
    EXPECT_TRUE(ewk_policy_decision_frame_get(policy_decision));
    owner->EventLoopStop(Success);
  }
};

TEST_F(utc_blink_ewk_policy_decision_frame_get, NAVIGATION_DECIDE)
{
  evas_object_smart_callback_auto scb(GetEwkWebView(), "policy,navigation,decide", frame_check, this);
  std::string resurl = GetResourceUrl("ewk_policy_decision/iframe.html");
  ASSERT_NE(0, resurl.length());
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), resurl.c_str()));
  // Wait for stop from main frame
  ASSERT_EQ(Success, EventLoopStart());

  // Wait for second stop from sub frame
  ASSERT_EQ(Success, EventLoopStart());
}

TEST_F(utc_blink_ewk_policy_decision_frame_get, RESPONSE_DECIDE)
{
  evas_object_smart_callback_auto scb(GetEwkWebView(), "policy,response,decide", frame_check, this);

  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), "http://www.w3schools.com/tags/tryit.asp?filename=tryhtml_iframe"));
  // Wait for stop from main frame
  ASSERT_EQ(Success, EventLoopStart());

  // Wait for second stop from sub frame
  ASSERT_EQ(Success, EventLoopStart());
}

TEST_F(utc_blink_ewk_policy_decision_frame_get, NEWWINDOW_DECIDE)
{
  evas_object_smart_callback_auto scb(GetEwkWebView(), "policy,newwindow,decide", frame_check, this);

  std::string resurl = GetResourceUrl("ewk_policy_decision/mainframe_newwindow.html");
  ASSERT_NE(0, resurl.length());
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), resurl.c_str()));
  // Wait for stop from main frame
  ASSERT_EQ(Success, EventLoopStart());

  // Wait for second stop from sub frame
  ASSERT_EQ(Success, EventLoopStart());
}

TEST_F(utc_blink_ewk_policy_decision_frame_get, INVALID_ARGS)
{
  EXPECT_FALSE(ewk_policy_decision_frame_get(NULL));
}
