// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_notification_icon_url_get : public utc_blink_ewk_base {
protected:
  utc_blink_ewk_notification_icon_url_get() : icon_url(NULL) {}

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

    utc_blink_ewk_notification_icon_url_get *owner = NULL;
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

    utc_blink_ewk_notification_icon_url_get* owner = static_cast<utc_blink_ewk_notification_icon_url_get*>(data);
    //call ewk_notification API
    owner->icon_url = ewk_notification_icon_url_get((Ewk_Notification*)event_info);
    utc_message("[notificationShow] :: icon_url = %s", owner->icon_url);
    if (owner->icon_url) {
      char* getIconName = strrchr((char*)owner->icon_url, '/');
      owner->icon_url = ++getIconName;
    }

    owner->EventLoopStop(Success);
  }

  // helper function
  bool click()
  {
    utc_message("[click] :: ");
    return ewk_view_script_execute(GetEwkWebView(), "document.getElementById(\"startButton\").click();", NULL, NULL) == EINA_TRUE;
  }

protected:
  static const char* const resource_relative_path;
  static const char* const notification_icon_ref;
  const char* icon_url;
};

const char* const utc_blink_ewk_notification_icon_url_get::resource_relative_path = "/common/sample_notification_2.html";
const char* const utc_blink_ewk_notification_icon_url_get::notification_icon_ref = "logo.png";

/**
* @brief Positive test case for ewk_notification_body_get(). Text returned by api is compared against expected text and result is set in notificationShow()
*/
TEST_F(utc_blink_ewk_notification_icon_url_get, POS_TEST)
{
  std::string resource_url = GetResourceUrl(resource_relative_path);
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), resource_url.c_str()));
  ASSERT_EQ(Success, EventLoopStart());
  ASSERT_TRUE(icon_url);
  ASSERT_STREQ(icon_url, notification_icon_ref);
}

/**
* @brief Checking whether function works properly in case of NULL of a webview.
*/
TEST_F(utc_blink_ewk_notification_icon_url_get, NEG_TEST)
{
  const char* iconURL = ewk_notification_icon_url_get(NULL);
  EXPECT_EQ(NULL, iconURL);
}
