// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_security_origin_protocol_get : public utc_blink_ewk_base
{
protected:
  /* Startup and cleanup functions */
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

  /* Callback for "notification,permission,request" */
  static void notificationPermissionRequest(void* data, Evas_Object* webview, void* event_info)
  {
    utc_message("[notificationPermissionRequest] :: \n");

    if (!event_info)
      utc_fail();

    //allow the notification
    ewk_notification_permission_request_set((Ewk_Notification_Permission_Request*)event_info, EINA_TRUE);
  }

   /* Callback for "notification,show" */
  static void notificationShow(void* data, Evas_Object* webview, void* event_info)
  {
    utc_message("[notificationShow] :: [Check Origin Protocal Get]\n");

    if (!event_info || !data)
      utc_fail();

    utc_blink_ewk_security_origin_protocol_get* owner = static_cast<utc_blink_ewk_security_origin_protocol_get*>( data );
    MainLoopResult result=Failure;
    Ewk_Notification* notification = static_cast<Ewk_Notification*>( event_info );
    if (notification) {
      const Ewk_Security_Origin *org = ewk_notification_security_origin_get((const Ewk_Notification*)notification);
      if (org) {
        const char* protocol = ewk_security_origin_protocol_get(org);
        if (protocol && !strcmp(protocol, ""))
          result = Success;
      }
    }
    owner->EventLoopStop(result);
  }
};

/**
 * @brief Positive test case for ewk_security_origin_protocol_get().
 */
TEST_F(utc_blink_ewk_security_origin_protocol_get, POS_TEST)
{
  if(!ewk_view_url_set(GetEwkWebView(),GetResourceUrl("common/sample_notification_1.html").c_str()))
    utc_fail();

  utc_check_eq(EventLoopStart(), Success);
}

/**
 * @brief Checking whether function works properly in case of NULL as origin.
 */
TEST_F(utc_blink_ewk_security_origin_protocol_get, NEG_TEST)
{
  const char* const protocol = ewk_security_origin_protocol_get(NULL);
  utc_check_false(protocol);
}
