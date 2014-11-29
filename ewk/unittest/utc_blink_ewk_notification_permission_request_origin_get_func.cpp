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

class utc_blink_ewk_notification_permission_request_origin_get : public utc_blink_ewk_base {
protected:
  static void script_execute_callback(Evas_Object* webview, const char* result_value, void* user_data)
  {
    utc_message("execution result: %s", result_value);
  }

  /* Callback for "load,finished" */
  void LoadFinished(Evas_Object* webview)
  {
    utc_message("[loadFinished] :: ");
    ewk_view_script_execute(webview, "noti();", script_execute_callback, this);
  }

  /* Callback for "notification,permission,request" */
  static void notificationPermissionRequest(void* data, Evas_Object* webview, void* event_info)
  {
    utc_message("[notificationPermissionRequest] :: ");
    ASSERT_TRUE(data != NULL);
    utc_blink_ewk_notification_permission_request_origin_get *owner = static_cast<utc_blink_ewk_notification_permission_request_origin_get *>(data);

    MainLoopResult res = Failure;
    Ewk_Notification_Permission_Request *request = (Ewk_Notification_Permission_Request*)event_info;
    const Ewk_Security_Origin* origin = ewk_notification_permission_request_origin_get(request);

    if (origin) {
      const char* host = ewk_security_origin_host_get(origin);
      const char* protocol = ewk_security_origin_protocol_get(origin);
      uint16_t port = ewk_security_origin_port_get(origin);
      utc_message("__actual__\nhost: %s\tprotocol: %s\tport: %d", host, protocol, port);
      utc_message("__expected__\nhost: %s\tprotocol: %s\tport: %d", expected_origin_host, expected_origin_protocol, expected_origin_port);

      if (!strcmp(protocol, expected_origin_protocol) && !strcmp(host, expected_origin_host) && port == expected_origin_port) {
        res = Success;
      }
    }

    //allow the notification
    ewk_notification_permission_request_set(request, EINA_TRUE);
    owner->EventLoopStop(res);
  }

  /* Startup and cleanup functions */
  virtual void PostSetUp()
  {
    ewk_view_mouse_events_enabled_set(GetEwkWebView(), EINA_TRUE);
    evas_object_smart_callback_add(GetEwkWebView(), "notification,permission,request", notificationPermissionRequest, this);
  }

  virtual void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(), "notification,permission,request", notificationPermissionRequest);
  }

protected:
  static const char* const resource_file;
  static const char* const expected_origin_protocol;
  static const char* const expected_origin_host;
  static const uint16_t expected_origin_port;
};

const char* const utc_blink_ewk_notification_permission_request_origin_get::resource_file = "common/sample_notification_3.html";
const char* const utc_blink_ewk_notification_permission_request_origin_get::expected_origin_protocol = "";
const char* const utc_blink_ewk_notification_permission_request_origin_get::expected_origin_host = "";
const uint16_t utc_blink_ewk_notification_permission_request_origin_get::expected_origin_port = 65535;


/**
 * @brief Positive test case for ewk_notification_permission_request_set function
 */
TEST_F(utc_blink_ewk_notification_permission_request_origin_get, POS_TEST)
{
  /* TODO: this code should use ewk_notification_permission_request_set and check its behaviour.
  Results should be reported using one of utc_ macros */

  ASSERT_TRUE(ewk_view_url_set(GetEwkWebView(), GetResourceUrl(resource_file).c_str()));
  ASSERT_EQ(Success, EventLoopStart());
}

/**
 * @brief Tests whether function works properly in case of NULL value pass.
 */
TEST_F(utc_blink_ewk_notification_permission_request_origin_get, NEG_TEST)
{
  /* TODO: this code should use ewk_notification_cached_permissions_set and check its behaviour.
  Results should be reported using one of utc_ macros */
  ewk_notification_permission_request_set(NULL,0);

  // If  NULL argument passing wont give segmentation fault negative test case will pass
  utc_pass();
}
