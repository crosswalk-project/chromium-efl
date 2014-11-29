/*
 * chromium EFL
 *
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_notification_policies_removed : public utc_blink_ewk_base {
protected:
  utc_blink_ewk_notification_policies_removed()
    : utc_blink_ewk_base()
    , notificationsShowed(0)
    , notificationPermissionRequestCalled(0)
  {
  }

  static void scriptExecuted(Evas_Object* o, const char* result, void* data)
  {
    utc_message("[script_executed] :: \n");
  }

  static Eina_Bool scriptExecuteWrapper(void* data)
  {
    if (data) {
      const char* const getPermissionRequestScript =
      "window.webkitNotifications.requestPermission(function(){"
      "window.webkitNotifications.createNotification(\"icon\", \"title\", \"body\").show();"
      "})";
      utc_message("[script_execute_wrapper] :: \n");
      utc_blink_ewk_notification_policies_removed *ut = static_cast<utc_blink_ewk_notification_policies_removed*>(data);
      Eina_Bool result = ewk_view_script_execute(ut->GetEwkWebView(), getPermissionRequestScript, scriptExecuted, 0);
      if (result == EINA_FALSE) {
        ut->EventLoopStop(utc_blink_ewk_base::Failure);
      }
    }
    return ECORE_CALLBACK_DONE;
  }

  static void executeScript(void* data)
  {
    ecore_timer_add(3, scriptExecuteWrapper, data);
  }

  static void notificationPermissionRequest(void* data, Evas_Object* webview, void* event_info)
  {
    utc_message("[notificationPermissionRequest] :: \n");
    ASSERT_TRUE(data);

    Ewk_Context* context = ewk_view_context_get(webview);
    utc_blink_ewk_notification_policies_removed *ut = static_cast<utc_blink_ewk_notification_policies_removed*>(data);

    if (!event_info || !context) {
      ut->EventLoopStop(utc_blink_ewk_base::Failure);
      return;
    }

    //allow the notification
    ewk_notification_permission_request_set(static_cast<Ewk_Notification_Permission_Request*>(event_info), EINA_TRUE);

    ut->notificationPermissionRequestCalled++;
    if (ut->notificationPermissionRequestCalled >= 2)
      ut->EventLoopStop(utc_blink_ewk_base::Success);
  }

  static void notificationShow(void* data, Evas_Object* webview, void* event_info)
  {
    utc_message("[notificationShow] \n");
    ASSERT_TRUE(data);

    Ewk_Context* context = ewk_view_context_get(webview);
    utc_blink_ewk_notification_policies_removed *ut = static_cast<utc_blink_ewk_notification_policies_removed*>(data);

    if (!event_info || !context) {
      ut->EventLoopStop(utc_blink_ewk_base::Failure);
      return;
    }

    ut->notificationsShowed++;
    if (ut->notificationsShowed >= 2) {
      ut->EventLoopStop(utc_blink_ewk_base::Success);
      return;
    }

    Eina_List* origins_list = NULL;
    const Ewk_Security_Origin* origin = ewk_notification_security_origin_get((Ewk_Notification*)event_info);
    origins_list = eina_list_append(origins_list, origin);

    ewk_notification_policies_removed(context, origins_list);

    executeScript(ut);

    eina_list_free(origins_list);
  }

  void LoadFinished(Evas_Object* webview)
  {
    executeScript(this);
  }

  virtual void PostSetUp()
  {
    evas_object_smart_callback_add(GetEwkWebView(), "notification,permission,request", notificationPermissionRequest, this);
    evas_object_smart_callback_add(GetEwkWebView(), "notification,show", notificationShow, this);
  }

  virtual void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(), "notification,permission,request", notificationPermissionRequest);
    evas_object_smart_callback_del(GetEwkWebView(), "notification,show", notificationShow);
  }

  static const char* const url;
  int notificationsShowed;
  int notificationPermissionRequestCalled;

};

const char* const utc_blink_ewk_notification_policies_removed::url = "http://www.example.com/";

/**
* @brief Positive test case for ewk_notification_policies_removed()
*/
TEST_F(utc_blink_ewk_notification_policies_removed, POS_TEST)
{
  utc_message("[loadURL] :: \n");
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), url));

  ASSERT_EQ(Success, EventLoopStart());
  ASSERT_EQ(notificationPermissionRequestCalled, 2);
}

/**
* @brief Checking whether function works properly in case of NULL value pass
*/
TEST_F(utc_blink_ewk_notification_policies_removed, NEG_TEST)
{
  ewk_notification_policies_removed(NULL, NULL);
  /* If NULL argument passing won't give segmentation fault negative test case will pass */
}
