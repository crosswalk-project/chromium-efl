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

/* Define those macros _before_ you include the utc_blink_ewk.h header file. */
#include "utc_blink_ewk.h"

#define SAMPLE_HTML_FILE                ("/common/sample_notification_2.html")

static Eina_Bool isFailed = EINA_FALSE;
static Eina_Bool notificationPermissionFirsttime  = EINA_FALSE;
static Eina_Bool notificationPermissionSecondtime = EINA_FALSE;

extern struct Ewk_Test_View test_view;
class utc_blink_ewk_notification_cached_permissions_set : public ::testing::Test {
protected:
  utc_blink_ewk_notification_cached_permissions_set() {
  }

  virtual ~utc_blink_ewk_notification_cached_permissions_set() {
  }

  /* Load specified url */
  static void loadURL(const char* url)
  {
    char* full_path = generate_full_resource_path(url);
    printf(full_path);

    if (!full_path)
      utc_fail();

    Eina_Bool result = ewk_view_url_set(test_view.webview, full_path);
    free(full_path);

    if (!result)
      utc_fail();
  }

  /* Callback for "load,finished" */
  static void loadFinished(void* data, Evas_Object* webview, void* event_info)
  {
    utc_blink_main_loop_quit();
  }

  /* Callback for load error */
  static void loadError(void* data, Evas_Object* webview, void* event_info)
  {
    isFailed = EINA_TRUE;
    utc_blink_main_loop_quit();
  }

  /* Callback for "notification,permission,request" */
  static void notificationPermissionRequest(void* data, Evas_Object* webview, void* event_info)
  {
    Ewk_Context *context = ewk_view_context_get(webview);
    if (!event_info || !context) {
      isFailed = EINA_TRUE;
      utc_blink_main_loop_quit();
      return;
    }

    //allow the notification
    ewk_notification_permission_request_set((Ewk_Notification_Permission_Request*)event_info, EINA_TRUE);

    if (!notificationPermissionFirsttime) {
      notificationPermissionFirsttime = EINA_TRUE;
    }
    else {
      notificationPermissionSecondtime = EINA_TRUE;
      return;
    }

    Eina_List *notification_list = NULL;
    Ewk_Notification_Permission* ewk_notification_p = (Ewk_Notification_Permission*) malloc(sizeof(Ewk_Notification_Permission));

    ewk_notification_p->origin  =  generate_full_resource_path(SAMPLE_HTML_FILE);
    ewk_notification_p->allowed = TRUE;

    notification_list = eina_list_append(notification_list, ewk_notification_p);
    ewk_notification_cached_permissions_set(context,notification_list);
  }

  /* Callback for "notification,show" */
  static void notificationShow(void* data, Evas_Object* webview, void* event_info)
  {
    if (!event_info)
    {
      isFailed = EINA_TRUE;
      utc_blink_main_loop_quit();
      return;
    }
  }

  /* Startup and cleanup functions */
  virtual void SetUp()
  {
    utc_blink_ewk_test_init();

    evas_object_smart_callback_add(test_view.webview, "notification,permission,request", notificationPermissionRequest, NULL);
    evas_object_smart_callback_add(test_view.webview, "notification,show", notificationShow, NULL);
    evas_object_smart_callback_add(test_view.webview, "load,finished", loadFinished, NULL);
    evas_object_smart_callback_add(test_view.webview, "load,error", loadError, NULL);
  }

  virtual void TearDown()
  {
    evas_object_smart_callback_del(test_view.webview, "notification,permission,request", notificationPermissionRequest);
    evas_object_smart_callback_del(test_view.webview, "notification,show", notificationShow);
    evas_object_smart_callback_del(test_view.webview, "load,finished", loadFinished);
    evas_object_smart_callback_del(test_view.webview, "load,error", loadError);

    utc_blink_ewk_test_end();
  }
};

/**
 * @brief Positive test case for ewk_notification_cached_permissions_set()
 */
TEST_F(utc_blink_ewk_notification_cached_permissions_set, POS_TEST)
{
  /* TODO: this code should use ewk_notification_cached_permissions_set and check its behaviour.
  Results should be reported using one of utc_ macros */
  isFailed = EINA_FALSE;

  loadURL(SAMPLE_HTML_FILE);
  utc_blink_main_loop_begin();

  if (isFailed)
    utc_fail();

  // wait a moment
  sleep(6);

  utc_check_eq(notificationPermissionSecondtime, EINA_FALSE);
}

/**
 * @brief Checking whether function works properly in case of NULL value pass.
 */
TEST_F(utc_blink_ewk_notification_cached_permissions_set, NEG_TEST)
{
  /* TODO: this code should use ewk_notification_cached_permissions_set and check its behaviour.
  Results should be reported using one of utc_ macros */
  ewk_notification_cached_permissions_set(NULL,NULL);
  // If  NULL argument passing wont give segmentation fault negative test case will pass
  utc_pass();
}
