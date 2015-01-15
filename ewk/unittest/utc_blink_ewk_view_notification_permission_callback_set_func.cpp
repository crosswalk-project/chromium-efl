// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_view_notification_permission_callback_set : public utc_blink_ewk_base
{
 protected:
  utc_blink_ewk_view_notification_permission_callback_set() : called(false) {}

  void LoadFinished(Evas_Object *o)
  {
    EventLoopStop(Success);
  }

  static Eina_Bool notificationPermissionRequest(Evas_Object* webview, Ewk_Notification_Permission_Request* request, void* data)
  {
    utc_blink_ewk_view_notification_permission_callback_set *owner = NULL;
    OwnerFromVoid(data, &owner);
    owner->called = true;
    owner->EventLoopStop(Success);
    return EINA_TRUE;
  }

  // helper function
  bool click()
  {
    utc_message("[click] :: ");
    return ewk_view_script_execute(GetEwkWebView(), "document.getElementById(\"startButton\").click();", NULL, NULL) == EINA_TRUE;
  }

 protected:
  bool called;
  static const char* const sample;
};

const char* const utc_blink_ewk_view_notification_permission_callback_set::sample = "common/sample_notification_2.html";

TEST_F(utc_blink_ewk_view_notification_permission_callback_set, POS_TEST)
{
  std::string resource_url = GetResourceUrl(sample);
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), resource_url.c_str()));
  ASSERT_EQ(Success, EventLoopStart());

  ASSERT_TRUE(click());
  ASSERT_EQ(Timeout, EventLoopStart(5.0));

  ASSERT_FALSE(called);

  ewk_view_notification_permission_callback_set(GetEwkWebView(), notificationPermissionRequest, this);

  ASSERT_TRUE(click());
  ASSERT_EQ(Success, EventLoopStart());

  ASSERT_TRUE(called);
  called = false;

  ewk_view_notification_permission_callback_set(GetEwkWebView(), NULL, this);

  ASSERT_TRUE(click());
  ASSERT_EQ(Timeout, EventLoopStart(5.0));

  ASSERT_FALSE(called);
}

TEST_F(utc_blink_ewk_view_notification_permission_callback_set, NEG_TEST)
{
  ewk_view_notification_permission_callback_set(NULL, NULL, NULL);
}
