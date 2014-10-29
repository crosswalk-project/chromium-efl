// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_notification_clicked : public utc_blink_ewk_base
{
 protected:
  /* Callback for "notification,permission,request" */
  static void notificationPermissionRequest(void* data, Evas_Object* webview, void* event_info)
  {
    utc_message("[notificationPermissionRequest] :: \n");
    if (!event_info) {
      FAIL();
    }
    //allow the notification
    ewk_notification_permission_request_set(static_cast<Ewk_Notification_Permission_Request*>(event_info), EINA_TRUE);
  }

  /* Callback for "notification,show" */
  static void notificationShow(void* data, Evas_Object* webview, void* event_info)
  {
    utc_message("[notificationShow] :: \n");

    Ewk_Context* context = ewk_view_context_get(webview);
    if (!event_info || !context) {
      utc_message("event_info: %p\ncontext: %p", event_info, context);
      FAIL();
    }

    if (!data) {
      FAIL();
    }

    utc_blink_ewk_notification_clicked* owner = static_cast<utc_blink_ewk_notification_clicked*>(data);

    uint64_t notification_id = ewk_notification_id_get(static_cast<Ewk_Notification*>(event_info));
    ewk_notification_clicked(context, notification_id);
    owner->EventLoopStop(Success);;
  }

  /* Startup function */
  virtual void PostSetUp()
  {
    evas_object_smart_callback_add(GetEwkWebView(), "notification,permission,request", notificationPermissionRequest, this);
    evas_object_smart_callback_add(GetEwkWebView(), "notification,show", notificationShow, this);
  }

  /* Cleanup function */
  virtual void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(), "notification,permission,request", notificationPermissionRequest);
    evas_object_smart_callback_del(GetEwkWebView(), "notification,show", notificationShow);
  }

protected:
  static const char* const resource_relative_path;
};

const char* const utc_blink_ewk_notification_clicked::resource_relative_path = "/common/sample_notification_1.html";

/**
* @brief Positive test case for ewk_notification_clicked()
*/
TEST_F(utc_blink_ewk_notification_clicked, POS_TEST)
{
  std::string resource_url = GetResourceUrl(resource_relative_path);

  if (!ewk_view_url_set(GetEwkWebView(), resource_url.c_str())) {
    FAIL();
  }

  EXPECT_EQ(Success, EventLoopStart());
}

/**
* @brief Checking whether function works properly in case of NULL value pass
*/
TEST_F(utc_blink_ewk_notification_clicked, NEG_TEST)
{
  ewk_notification_clicked(NULL, 0);
  /* If NULL argument passing won't give segmentation fault negative test case will pass */
  SUCCEED();
}
