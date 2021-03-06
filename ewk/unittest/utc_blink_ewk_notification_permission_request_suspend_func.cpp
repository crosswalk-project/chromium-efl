// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_notification_permission_request_suspend : public utc_blink_ewk_base {
protected:
  utc_blink_ewk_notification_permission_request_suspend() {}
  /* Callback for notification permission request */
  static Eina_Bool notificationPermissionRequest(Evas_Object* webview, Ewk_Notification_Permission_Request* request, void* data)
  {
    Ewk_Context *context = ewk_view_context_get(webview);
    utc_blink_ewk_notification_permission_request_suspend *owner = NULL;
    OwnerFromVoid(data, &owner);
    if(!request || !context) {
      utc_message("event_info: %p\ncontext: %p", request, context);
      owner->EventLoopStop(Failure);
      return EINA_FALSE;
    }

    //allow the notification
    ewk_notification_permission_request_suspend(request);
    ewk_notification_permission_reply(request, EINA_TRUE);
    owner->EventLoopStop(Success);
    return EINA_TRUE;
  }

  static void cb_console_message(utc_blink_ewk_notification_permission_request_suspend *owner, Evas_Object *obj, void *event_info)
  {
    Ewk_Console_Message* console = static_cast<Ewk_Console_Message*>(event_info);
    const char *cmsg = ewk_console_message_text_get(console);
    if (!cmsg) {
      return;
    }
    std::string msg(cmsg);
    std::string::size_type pos = msg.find(':');
    if (pos == std::string::npos) {
      return;
    }
    if (msg.substr(0, pos) == "event_loop_stop") {
      owner->EventLoopStop(Success);
    }
  }

  /* Startup function */
  bool click()
  {
    utc_message("[click] :: ");
    return ewk_view_script_execute(GetEwkWebView(), "document.getElementById(\"startButton\").click();", NULL, NULL) == EINA_TRUE;
  }

  /* Startup and cleanup functions */
  virtual void PostSetUp()
  {
    ewk_view_notification_permission_callback_set(GetEwkWebView(), notificationPermissionRequest, this);
    evas_object_smart_callback_add(GetEwkWebView(), "console,message",
                                   ToSmartCallback(cb_console_message), this);
  }

  virtual void PreTearDown()
  {
    ewk_view_notification_permission_callback_set(GetEwkWebView(), NULL, NULL);
    evas_object_smart_callback_del(GetEwkWebView(), "console,message",
                                   ToSmartCallback(cb_console_message));
  }

protected:
  static const char* const resource_relative_path;
  static const char* const notification_title_ref;
};

const char* const utc_blink_ewk_notification_permission_request_suspend::resource_relative_path = "/common/sample_notification_2.html";
const char* const utc_blink_ewk_notification_permission_request_suspend::notification_title_ref = "Notification Title";

/**
* @brief Positive test case for ewk_notification_permission_request_suspened()
*/
TEST_F(utc_blink_ewk_notification_permission_request_suspend, POS_TEST)
{
  std::string resource_url = GetResourceUrl(resource_relative_path);
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), resource_url.c_str()));
  ASSERT_EQ(Success, EventLoopStart());
  ASSERT_TRUE(click());
  ASSERT_EQ(Success, EventLoopStart());

}

/**
* @brief Checking whether function works properly in case of NULL value pass
*/
TEST_F(utc_blink_ewk_notification_permission_request_suspend, NEG_TEST)
{
  ewk_notification_permission_request_suspend(NULL);
  // If  NULL argument passing wont give segmentation fault negative test case will pass
  SUCCEED();
}
