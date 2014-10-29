// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_notification_security_origin_get_func : public utc_blink_ewk_base
{
protected:

  /* Callback for "notification,permission,request" */
  static void notificationPermissionRequest(void* owner, Evas_Object* webview, void* event_info)
  {
    utc_message("[notificationPermissionRequest] ::");
    ASSERT_TRUE(NULL != event_info);

    //allow the notification
    ewk_notification_permission_request_set(static_cast<Ewk_Notification_Permission_Request*>(event_info), EINA_TRUE);
  }

  /* Callback for "notification,show" */
  static void notificationShow(void* owner, Evas_Object* webview, void* event_info)
  {
    utc_message("[notificationShow] ::");
    ASSERT_TRUE(event_info != NULL);
    ASSERT_TRUE(owner != NULL);

    //call ewk_notification API
    const Ewk_Security_Origin* origin = ewk_notification_security_origin_get(static_cast<Ewk_Notification*>(event_info));
    ASSERT_TRUE(origin != NULL);

    ASSERT_STREQ(ewk_security_origin_protocol_get(origin), "");
    ASSERT_STREQ(ewk_security_origin_host_get(origin), "");
    ASSERT_EQ(ewk_security_origin_port_get(origin), 0);
    static_cast<utc_blink_ewk_notification_security_origin_get_func*>(owner)->EventLoopStop(Success);
  }

  void PostSetUp()
  {
    evas_object_smart_callback_add(GetEwkWebView(), "notification,permission,request", notificationPermissionRequest, this);
    evas_object_smart_callback_add(GetEwkWebView(), "notification,show", notificationShow, this);
  }

  void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(), "notification,permission,request", notificationPermissionRequest);
    evas_object_smart_callback_del(GetEwkWebView(), "notification,show", notificationShow);
  }
};

/**
* @brief Positive test case for ewk_notification_security_origin_get().
*/
TEST_F(utc_blink_ewk_notification_security_origin_get_func, POS_TEST)
{
  ASSERT_TRUE(ewk_view_url_set(GetEwkWebView(), GetResourceUrl("common/sample_notification_1.html").c_str()));
  EXPECT_EQ(Success, EventLoopStart());
}

/**
* @brief Checking whether function works properly in case of NULL of a webview.
*/
TEST_F(utc_blink_ewk_notification_security_origin_get_func, InvalidArg)
{
  EXPECT_EQ(NULL, ewk_notification_security_origin_get(NULL));
}
