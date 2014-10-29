// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_frame_is_main_frame: public utc_blink_ewk_base
{
protected:
  static void main_frame_check(void* data, Evas_Object* webview, void* event_info)
  {
    utc_message("  [ %s ]", __PRETTY_FUNCTION__);
    CheckIsMainFrame(data, event_info, EINA_TRUE);
  }

  static void sub_frame_check(void* data, Evas_Object* webview, void* event_info)
  {
    CheckIsMainFrame(data, event_info, EINA_FALSE);
  }

  static void CheckIsMainFrame(void* data, void* event_info, Eina_Bool expected_value)
  {
    utc_message("  [ %s ]", __PRETTY_FUNCTION__);
    ASSERT_TRUE(data);
    ASSERT_TRUE(event_info);

    utc_blink_ewk_frame_is_main_frame* owner = NULL;
    OwnerFromVoid(data, &owner);

    Ewk_Policy_Decision* policy_decision = static_cast<Ewk_Policy_Decision*>(event_info);
    Ewk_Frame_Ref frame = ewk_policy_decision_frame_get(policy_decision);

    ASSERT_TRUE(frame);
    if (ewk_frame_is_main_frame(frame) == expected_value) {
      owner->EventLoopStop(Success);
    }
  }
};

TEST_F(utc_blink_ewk_frame_is_main_frame, MAIN_FRAME)
{
  evas_object_smart_callback_auto scb(GetEwkWebView(), "policy,navigation,decide", main_frame_check, this);

  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), "http://www.google.com"));
  ASSERT_EQ(Success, EventLoopStart());
}

TEST_F(utc_blink_ewk_frame_is_main_frame, SUB_FRAME)
{
  evas_object_smart_callback_auto scb(GetEwkWebView(), "policy,navigation,decide", sub_frame_check, this);
  std::string resurl = GetResourceUrl("ewk_frame/iframetest.html");
  ASSERT_NE(resurl.length(), 0);

  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), resurl.c_str()));
  ASSERT_EQ(Success, EventLoopStart());
}

TEST_F(utc_blink_ewk_frame_is_main_frame, INVALID_ARGS)
{
  EXPECT_EQ(EINA_FALSE, ewk_frame_is_main_frame(NULL));
}
