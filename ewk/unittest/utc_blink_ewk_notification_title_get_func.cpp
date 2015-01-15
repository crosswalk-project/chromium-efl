// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/* Define those macros _before_ you include the utc_blink_ewk.h header file. */
#include "utc_blink_ewk_base.h"

class utc_blink_ewk_notification_title_get : public utc_blink_ewk_base {
protected:
  utc_blink_ewk_notification_title_get() : title(NULL) {}

  /* Callback for notification permission request */
  static Eina_Bool notificationPermissionRequest(Evas_Object* webview, Ewk_Notification_Permission_Request* request, void* data)
  {
    utc_message("[notificationPermissionRequest] :: ");
    utc_blink_ewk_notification_title_get *owner = NULL;
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
      FAIL();
    }

    if (!data) {
      FAIL();
    }

    utc_blink_ewk_notification_title_get* owner = static_cast<utc_blink_ewk_notification_title_get*>(data);

    //call ewk_notification API
    owner->title = ewk_notification_title_get((Ewk_Notification*)event_info);
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
  static const char* const notification_title_ref;
  const char* title;
};

const char* const utc_blink_ewk_notification_title_get::resource_relative_path = "/common/sample_notification_2.html";
const char* const utc_blink_ewk_notification_title_get::notification_title_ref = "Notification Title";

/**
* @brief Positive test case for ewk_notification_title_get(). Text returned by api is compared against expected text and result is set in notificationShow()
*/
TEST_F(utc_blink_ewk_notification_title_get, POS_TEST)
{
  std::string resource_url = GetResourceUrl(resource_relative_path);

  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), resource_url.c_str()));
  ASSERT_EQ(Success, EventLoopStart());
  ASSERT_TRUE(title);
  ASSERT_STREQ(notification_title_ref, title);
}

/**
* @brief Checking whether function works properly in case of NULL of a webview.
*/
TEST_F(utc_blink_ewk_notification_title_get, NEG_TEST)
{
  const char* getTitle = ewk_notification_title_get(NULL);
  utc_check_eq(NULL, getTitle);
}
