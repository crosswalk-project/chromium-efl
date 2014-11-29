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

#define SAMPLE_HTML_FILE                ("/common/sample_notification_1.html")
static Eina_Bool result = EINA_FALSE;

extern struct Ewk_Test_View test_view;

class utc_blink_ewk_notification_clicked : public ::testing::Test {
 protected:
  utc_blink_ewk_notification_clicked() {
  }

  virtual ~utc_blink_ewk_notification_clicked() {
  }

  /* Load specified url */
  static void loadURL(const char* url)
  {
    utc_message("[loadURL] :: \n");

    char* full_path = generate_full_resource_path(url);
    if (!full_path)
      utc_fail();

    Eina_Bool result = ewk_view_url_set(test_view.webview, full_path);
    free(full_path);

    if (!result)
      utc_fail();
  }

  /* Callback for load error */
  static void loadError(void* data, Evas_Object* webview, void* event_info)
  {
    utc_message("[loadError] :: \n");
    utc_blink_main_loop_quit();
  }

  /* Callback for "notification,permission,request" */
  static void notificationPermissionRequest(void* data, Evas_Object* webview, void* event_info)
  {
    utc_message("[notificationPermissionRequest] :: \n");

    if (!event_info)
      utc_fail();

    //allow the notification
    ewk_notification_permission_request_set((Ewk_Notification_Permission_Request*)event_info, EINA_TRUE);
  }

  /* Callback for "notification,show" */
  static void notificationShow(void* data, Evas_Object* webview, void* event_info)
  {
    utc_message("[notificationShow] :: \n");

    Ewk_Context *context = ewk_view_context_get(webview);
    if (!event_info || !context)
      utc_fail();

    //call ewk_notification API
    uint64_t notificationID = ewk_notification_id_get((Ewk_Notification*)event_info);
    ewk_notification_clicked(context, notificationID);
    result = EINA_TRUE;
    utc_blink_main_loop_quit();
  }

  /* Startup function */
  virtual void SetUp()
  {
    utc_blink_ewk_test_init();

    evas_object_smart_callback_add(test_view.webview, "notification,permission,request", notificationPermissionRequest, NULL);
    evas_object_smart_callback_add(test_view.webview, "notification,show", notificationShow, NULL);
    evas_object_smart_callback_add(test_view.webview, "load,error", loadError, NULL);
  }

  /* Cleanup function */
  virtual void TearDown()
  {
    evas_object_smart_callback_del(test_view.webview, "notification,permission,request", notificationPermissionRequest);
    evas_object_smart_callback_del(test_view.webview, "notification,show", notificationShow);
    evas_object_smart_callback_del(test_view.webview, "load,error", loadError);

    utc_blink_ewk_test_end();
  }
};

/**
* @brief Positive test case for ewk_notification_clicked()
*/
TEST_F(utc_blink_ewk_notification_clicked, POS_TEST)
{
  loadURL(SAMPLE_HTML_FILE);
  utc_blink_main_loop_begin();

  utc_check_eq(result, EINA_TRUE);
}

/**
* @brief Checking whether function works properly in case of NULL.
*/
TEST_F(utc_blink_ewk_notification_clicked, NEG_TEST)
{
  ewk_notification_clicked(NULL, 1234);
  /* If NULL argument passing won't give segmentation fault negative test case will pass */
  utc_pass();
}
