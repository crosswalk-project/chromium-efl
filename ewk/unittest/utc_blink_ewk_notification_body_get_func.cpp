// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_notification_body_get : public utc_blink_ewk_base {
protected:
  utc_blink_ewk_notification_body_get() : body(NULL) {}

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

  /* Callback for notification permission request */
  static Eina_Bool notificationPermissionRequest(Evas_Object* webview, Ewk_Notification_Permission_Request* request, void* data)
  {
    utc_message("[notificationPermissionRequest] :: ");

    utc_blink_ewk_notification_body_get *owner = NULL;
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
    utc_message("[notificationShow] :: ");

    if (!event_info) {
      utc_fail();
    }

    if (!data) {
      utc_fail();
    }

    utc_blink_ewk_notification_body_get* owner = static_cast<utc_blink_ewk_notification_body_get*>(data);

    //call ewk_notification API
    owner->body = strdup(ewk_notification_body_get((Ewk_Notification*)event_info));
    owner->EventLoopStop(Success);
  }

protected:
  static const char* const resource_relative_path;
  static const char* const notification_body_ref;
  char* body;
};

const char* const utc_blink_ewk_notification_body_get::resource_relative_path = "/common/sample_notification_2.html";
const char* const utc_blink_ewk_notification_body_get::notification_body_ref = "Notification body content";

/**
* @brief Positive test case for ewk_notification_body_get(). Text returned by api is compared against expected text and result is set in notificationShow()
*/
TEST_F(utc_blink_ewk_notification_body_get, POS_TEST)
{
  std::string resource_url = GetResourceUrl(resource_relative_path);
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), resource_url.c_str()));
  ASSERT_EQ(Success, EventLoopStart());
  ASSERT_TRUE(body);
  ASSERT_STREQ(body, notification_body_ref);
  free(body);
}

/**
* @brief Checking whether function works properly in case of NULL of a webview.
*/
TEST_F(utc_blink_ewk_notification_body_get, NEG_TEST)
{
  const char *bodyText = ewk_notification_body_get(NULL);
  utc_check_eq(NULL, bodyText);
}
