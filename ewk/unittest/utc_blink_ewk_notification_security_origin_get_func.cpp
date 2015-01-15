// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_notification_security_origin_get_func : public utc_blink_ewk_base
{
protected:
  utc_blink_ewk_notification_security_origin_get_func() {}

  /* Callback for notification permission request */
  static Eina_Bool notificationPermissionRequest(Evas_Object* webview, Ewk_Notification_Permission_Request* request, void* data)
  {
    utc_message("[notificationPermissionRequest] ::");
    utc_blink_ewk_notification_security_origin_get_func *owner = NULL;
    OwnerFromVoid(data, &owner);
    if (!request) {
      owner->EventLoopStop(Failure);
      return EINA_FALSE;
    }

    //allow the notification
    ewk_notification_permission_reply(request, EINA_TRUE);
    return EINA_TRUE;
  }

  /* Callback for "notification,show" */
  static void notificationShow(void* data, Evas_Object* webview, void* event_info)
  {
    utc_message("[notificationShow] ::");
    ASSERT_TRUE(event_info != NULL);
    ASSERT_TRUE(data != NULL);

    //call ewk_notification API
    const Ewk_Security_Origin* origin = ewk_notification_security_origin_get(static_cast<Ewk_Notification*>(event_info));
    ASSERT_TRUE(origin != NULL);
    utc_blink_ewk_notification_security_origin_get_func *owner = NULL;
    OwnerFromVoid(data, &owner);

    owner->protocol = ewk_security_origin_protocol_get(origin);
    owner->host = ewk_security_origin_host_get(origin);
    owner->port = ewk_security_origin_port_get(origin);

    owner->EventLoopStop(Success);
  }

  void PostSetUp()
  {
    ewk_view_notification_permission_callback_set(GetEwkWebView(), notificationPermissionRequest, this);
    evas_object_smart_callback_add(GetEwkWebView(), "notification,show", notificationShow, this);
  }

  void PreTearDown()
  {
    ewk_view_notification_permission_callback_set(GetEwkWebView(), NULL, NULL);
    evas_object_smart_callback_del(GetEwkWebView(), "notification,show", notificationShow);
  }

  // helper function
  bool click()
  {
    utc_message("[click] :: ");
    return ewk_view_script_execute(GetEwkWebView(), "document.getElementById(\"startButton\").click();", NULL, NULL) == EINA_TRUE;
  }

protected:
  const char* protocol;
  const char* host;
  uint16_t port;
};

/**
* @brief Positive test case for ewk_notification_security_origin_get().
*/
TEST_F(utc_blink_ewk_notification_security_origin_get_func, POS_TEST)
{
  std::string resource_url = GetResourceUrl("common/sample_notification_2.html");
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), resource_url.c_str()));
  ASSERT_EQ(Success, EventLoopStart());
  ASSERT_TRUE(protocol);
  ASSERT_TRUE(host);
  ASSERT_STREQ("", protocol);
  ASSERT_STREQ("", host);
  ASSERT_EQ(0, port);
}

/**
* @brief Checking whether function works properly in case of NULL of a webview.
*/
TEST_F(utc_blink_ewk_notification_security_origin_get_func, InvalidArg)
{
  EXPECT_EQ(NULL, ewk_notification_security_origin_get(NULL));
}
