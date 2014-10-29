// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_notification_closed : public utc_blink_ewk_base
{
 protected:
  /* Callback for "notification,permission,request" */
  static void notificationPermissionRequest(void* data, Evas_Object* webview, void* event_info)
  {
    utc_message("[notificationPermissionRequest] :: \n");
    if (!event_info)
        utc_fail();
    //allow the notification
    ewk_notification_permission_request_set(static_cast<Ewk_Notification_Permission_Request*>(event_info), EINA_TRUE);
  }

  /* Callback for "notification,show" */
  static void notificationShow(void* data, Evas_Object* webview, void* event_info)
  {
    utc_message("[notificationShow] :: \n");
    if(!data)
      utc_fail();
    utc_blink_ewk_notification_closed* owner=static_cast<utc_blink_ewk_notification_closed*>(data);

    Ewk_Context* context = ewk_view_context_get(webview);
    if (!event_info || !context)
      utc_fail();

    owner->old_notification = static_cast<Ewk_Notification*>(event_info);
    uint64_t notification_id = ewk_notification_id_get(owner->old_notification);
    ewk_notification_showed(context, notification_id);
    owner->EventLoopStop(Success);
  }

  /* Startup function */
  void PostSetUp()
  {
    old_notification=0;
    evas_object_smart_callback_add(GetEwkWebView(), "notification,permission,request", notificationPermissionRequest, this);
    evas_object_smart_callback_add(GetEwkWebView(), "notification,show", notificationShow, this);
  }

  /* Cleanup function */
  void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(), "notification,permission,request", notificationPermissionRequest);
    evas_object_smart_callback_del(GetEwkWebView(), "notification,show", notificationShow);
  }

protected:
  static const char*const sample;
  Ewk_Notification* old_notification;
};

const char*const utc_blink_ewk_notification_closed::sample="common/sample_notification_1.html";

/**
* @brief Positive test case for ewk_notification_showed()
*/
TEST_F(utc_blink_ewk_notification_closed, POS_TEST)
{
  if(!ewk_view_url_set(GetEwkWebView(), GetResourceUrl(sample).c_str()))
    utc_fail();

  if(!EventLoopStart())
     utc_fail();

  Eina_List *list = NULL;
  list =  eina_list_append(list, old_notification);
  ewk_view_notification_closed(GetEwkWebView(), list);
  utc_pass();
}

/**
* @brief Checking whether function works properly in case of NULL value pass
*/
TEST_F(utc_blink_ewk_notification_closed, NEG_TEST)
{
  ewk_notification_showed(NULL, 0);
  /* If NULL argument passing won't give segmentation fault negative test case will pass */
  utc_pass();
}
