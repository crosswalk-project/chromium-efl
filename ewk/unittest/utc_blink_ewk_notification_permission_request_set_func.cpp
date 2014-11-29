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

class utc_blink_ewk_notification_permission_request_set : public utc_blink_ewk_base {
protected:
  utc_blink_ewk_notification_permission_request_set()
  : utc_blink_ewk_base()
  , notificationPermissionFirsttime(EINA_FALSE)
  , notificationPermissionSecondtime(EINA_FALSE)
  {
  }

  virtual ~utc_blink_ewk_notification_permission_request_set() {
  }

  /* Callback for "notification,permission,request" */
  static void notificationPermissionRequest(void* data, Evas_Object* webview, void* event_info)
  {
    if (!data) {
      FAIL();
    }

    utc_blink_ewk_notification_permission_request_set* owner = static_cast<utc_blink_ewk_notification_permission_request_set*>(data);

    Ewk_Context *context = ewk_view_context_get(webview);
    if (!event_info || !context) {
      FAIL();
    }

    //allow the notification
    ewk_notification_permission_request_set((Ewk_Notification_Permission_Request*)event_info, EINA_TRUE);

    if (!owner->notificationPermissionFirsttime) {
      owner->notificationPermissionFirsttime = EINA_TRUE;
    } else {
      owner->notificationPermissionSecondtime = EINA_TRUE;
      owner->EventLoopStop(Failure);
    }
  }

  /* Callback for "notification,show" */
  static void notificationShow(void* data, Evas_Object* webview, void* event_info)
  {
    if (!event_info) {
      FAIL();
    }

    if (!data) {
      FAIL();
    }

    utc_blink_ewk_notification_permission_request_set* owner = static_cast<utc_blink_ewk_notification_permission_request_set*>(data);

    owner->EventLoopStop(Success);
  }

  /* Startup and cleanup functions */
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

protected:
  Eina_Bool notificationPermissionFirsttime;
  Eina_Bool notificationPermissionSecondtime;
  static const char* const resource_relative_path;
};

const char* const utc_blink_ewk_notification_permission_request_set::resource_relative_path = "/common/sample_notification_2.html";

/**
 * @brief Positive test case for ewk_notification_permission_request_set function
 */
TEST_F(utc_blink_ewk_notification_permission_request_set, POS_TEST)
{
  std::string resource_url = GetResourceUrl(resource_relative_path);

  if (!ewk_view_url_set(GetEwkWebView(), resource_url.c_str())) {
    FAIL();
  }
  /* TODO: this code should use ewk_notification_permission_request_set and check its behaviour.
  Results should be reported using one of utc_ macros */

  MainLoopResult loop_result = EventLoopStart();

  if (loop_result != Success) {
    FAIL();
  }

  EXPECT_EQ(notificationPermissionSecondtime, EINA_FALSE);
}

/**
 * @brief Tests whether function works properly in case of NULL value pass.
 */
TEST_F(utc_blink_ewk_notification_permission_request_set, NEG_TEST)
{
  /* TODO: this code should use ewk_notification_cached_permissions_set and check its behaviour.
  Results should be reported using one of utc_ macros */
  ewk_notification_permission_request_set(NULL,0);

  // If  NULL argument passing wont give segmentation fault negative test case will pass
  SUCCEED();
}
