// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_notification_showed : public utc_blink_ewk_base {
 protected:
  utc_blink_ewk_notification_showed() {}

  /* Callback for notification permission request */
  static Eina_Bool notificationPermissionRequest(Evas_Object* webview, Ewk_Notification_Permission_Request* request, void* data)
  {
    utc_message("[notificationPermissionRequest] :: \n");
    utc_blink_ewk_notification_showed *owner = NULL;
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
    utc_message("[notificationShow] :: \n");

    Ewk_Context* context = ewk_view_context_get(webview);
    if (!event_info || !context) {
      utc_message("event_info: %p\ncontext: %p", event_info, context);
      FAIL();
    }

    if (!data) {
      FAIL();
    }

    utc_blink_ewk_notification_showed* owner = static_cast<utc_blink_ewk_notification_showed*>(data);

    uint64_t notification_id = ewk_notification_id_get((Ewk_Notification*)event_info);
    ewk_notification_showed(context, notification_id);
    owner->EventLoopStop(Success);
  }

  /* Startup function */
  virtual void PostSetUp()
  {
    ewk_view_notification_permission_callback_set(GetEwkWebView(), notificationPermissionRequest, this);
    evas_object_smart_callback_add(GetEwkWebView(), "notification,show", notificationShow, this);
  }

  /* Cleanup function */
  virtual void PreTearDown()
  {
    ewk_view_notification_permission_callback_set(GetEwkWebView(), NULL, NULL);
    evas_object_smart_callback_del(GetEwkWebView(), "notification,show", notificationShow);
  }

protected:
  static const char* const resource_relative_path;
};

const char* const utc_blink_ewk_notification_showed::resource_relative_path = "/common/sample_notification_2.html";

/**
* @brief Positive test case for ewk_notification_showed()
*/
TEST_F(utc_blink_ewk_notification_showed, POS_TEST)
{
  std::string resource_url = GetResourceUrl(resource_relative_path);
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), resource_url.c_str()));
  ASSERT_EQ(Success, EventLoopStart());
}

/**
* @brief Checking whether function works properly in case of NULL value pass
*/
TEST_F(utc_blink_ewk_notification_showed, NEG_TEST)
{
  ewk_notification_showed(NULL, 0);
  /* If NULL argument passing won't give segmentation fault negative test case will pass */
  SUCCEED();
}
