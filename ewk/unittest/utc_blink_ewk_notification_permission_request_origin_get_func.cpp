// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_notification_permission_request_origin_get : public utc_blink_ewk_base {
protected:
  /* Callback for notification permission request */
  static Eina_Bool notificationPermissionRequest(Evas_Object* webview, Ewk_Notification_Permission_Request *request, void* data)
  {
    utc_message("[notificationPermissionRequest] :: ");
    utc_blink_ewk_notification_permission_request_origin_get *owner = static_cast<utc_blink_ewk_notification_permission_request_origin_get *>(data);
    if (!owner) {
      owner->EventLoopStop(Failure);
      return EINA_FALSE;
    }

    MainLoopResult res = Failure;
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
    ewk_notification_permission_reply(request, EINA_TRUE);
    owner->EventLoopStop(res);
    return EINA_TRUE;
  }

  /* Startup and cleanup functions */
  virtual void PostSetUp()
  {
    ewk_view_mouse_events_enabled_set(GetEwkWebView(), EINA_TRUE);
    ewk_view_notification_permission_callback_set(GetEwkWebView(), notificationPermissionRequest, this);
  }

  virtual void PreTearDown()
  {
    ewk_view_notification_permission_callback_set(GetEwkWebView(), NULL, NULL);
  }

protected:
  static const char* const resource_file;
  static const char* const expected_origin_protocol;
  static const char* const expected_origin_host;
  static const uint16_t expected_origin_port;
};

const char* const utc_blink_ewk_notification_permission_request_origin_get::resource_file = "common/sample_notification_2.html";
const char* const utc_blink_ewk_notification_permission_request_origin_get::expected_origin_protocol = "";
const char* const utc_blink_ewk_notification_permission_request_origin_get::expected_origin_host = "";
const uint16_t utc_blink_ewk_notification_permission_request_origin_get::expected_origin_port = 65535;

/**
 * @brief Positive test case for ewk_notification_permission_reply function
 */
TEST_F(utc_blink_ewk_notification_permission_request_origin_get, POS_TEST)
{
  /* TODO: this code should use ewk_notification_permission_reply and check its behaviour.
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
  ewk_notification_permission_reply(NULL,0);

  // If  NULL argument passing wont give segmentation fault negative test case will pass
  utc_pass();
}
