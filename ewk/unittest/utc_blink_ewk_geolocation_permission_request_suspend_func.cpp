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

#include "utc_blink_ewk_base.h"

static const char* PERMISSION_DENIED = "EWK Geolocation PERMISSION_DENIED";
static const char* POSITION_UNAVAILABLE = "EWK Geolocation POSITION_UNAVAILABLE";
static const char* GEOLOCATION_SUCCESS = "EWK Geolocation SUCCESS";

#define SAMPLE_HTML_FILE                 ("/common/sample_js_geolocation.html")

class utc_blink_ewk_geolocation_permission_request_suspend : public utc_blink_ewk_base {

protected:

  Eina_Bool call_request_suspend;

  void PostSetUp()
  {
    evas_object_smart_callback_add(GetEwkWebView(),"geolocation,permission,request", request_geolocation_permisson, this);
    evas_object_smart_callback_add(GetEwkWebView(), "title,changed", onTitleChanged, this);
  }

  void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(),"geolocation,permission,request", request_geolocation_permisson);
    evas_object_smart_callback_del(GetEwkWebView(), "title,changed", onTitleChanged);
  }

  void LoadFinished(Evas_Object* webview) {
    //EventLoopStop(utc_blink_ewk_base::Failure);
  }

  static Eina_Bool geolocation_permision_request_set_wrapper(void* permission_request)
  {
     Eina_Bool ret = ewk_geolocation_permission_request_set((Ewk_Geolocation_Permission_Request*)permission_request, EINA_TRUE);

     EXPECT_TRUE(ret);

     //return ECORE_CALLBACK_CANCEL to stop calling function by timer.
     return ECORE_CALLBACK_CANCEL;
  }

  static void request_geolocation_permisson(void* data, Evas_Object* webview, void* event_info)
  {
    utc_message("[request_geolocation_permisson] :: \n");
    utc_blink_ewk_geolocation_permission_request_suspend *owner = static_cast<utc_blink_ewk_geolocation_permission_request_suspend*>(data);
    Ewk_Geolocation_Permission_Request* permission_request = (Ewk_Geolocation_Permission_Request*)event_info;

    /*
     * If call_request_suspend is equal EINA_TRUE then
     * ewk_geolocation_permission_request_suspend is invoke, otherwise is not call.
     */
    if (owner->call_request_suspend) {

      ewk_geolocation_permission_request_suspend(permission_request);
      ecore_timer_add(2, geolocation_permision_request_set_wrapper, permission_request);
    }
  }

  static void onTitleChanged(void* userData, Evas_Object* webView, void* event_info)
  {
    utc_message("[onTitleChanged] :: \n");

    if (!userData) {
      FAIL();
    }

    utc_blink_ewk_geolocation_permission_request_suspend *owner = static_cast<utc_blink_ewk_geolocation_permission_request_suspend*>(userData);

    if (owner->call_request_suspend) {
      // Positive test:
      // In some situations although that geolocation permission request is set to EINA_TRUE,
      // geolocation position cannot be obtained because GPS signal is unavailable.
      // Thus both state POSITION_UNAVAILABLE and GEOLOCATION_SUCCESS are possible.
      // However if permission is denied then it is incorrect behaviour.
      if (!strcmp(POSITION_UNAVAILABLE, (char*)event_info) || !strcmp(GEOLOCATION_SUCCESS, (char*)event_info)) {

        owner->EventLoopStop(utc_blink_ewk_base::Success);       
      }
      else if (!strcmp(PERMISSION_DENIED, (char*)event_info)) {

        owner->EventLoopStop(utc_blink_ewk_base::Failure);
      }
    } else {
      // Negative test:
      // The permission should be denied.
      if (!strcmp(PERMISSION_DENIED, (char*)event_info)) {

        owner->EventLoopStop(utc_blink_ewk_base::Success);
      }
    }
  }
};

/**
* @brief Positive test whether geolocation permission request is successfully suspended.
* When geolocation permission request is suspended then ewk_geolocation_permission_request_set can be invoked
* after the request_geolocation_permisson callback. If geolocation permission request will be set to EINA_TRUE
* then geolocation position should be successfully obtained or if the current GPS position is unavailable then
* geolocation position should be UNAVAILABLE.
*/
TEST_F(utc_blink_ewk_geolocation_permission_request_suspend, POS_TEST)
{
//TODO: this code should use ewk_geolocation_permission_request_suspend and check its behaviour.
//Results should be reported using one of utc_ macros */
  call_request_suspend = EINA_TRUE;

  Eina_Bool result = ewk_view_url_set(GetEwkWebView(), GetResourceUrl(SAMPLE_HTML_FILE).c_str());
  if (!result)
    FAIL();

  utc_blink_ewk_base::MainLoopResult main_result = EventLoopStart();
  if (main_result != utc_blink_ewk_base::Success)
    FAIL();
}

/**
* @brief Negative test whether geolocation errorCallback is invoked when ewk_geolocation_permission_request_suspend is NOT called.
* When call_request_suspend is equal EINA_FALSE then ewk_geolocation_permission_request_suspend is NOT invoked in request_geolocation_permisson
* callback. Because geolocation permission request is NOT suspend, thus errorCallback with PERMISSION_DENIED error code should be invoked.
*/
TEST_F(utc_blink_ewk_geolocation_permission_request_suspend, NEG_TEST)
{
//TODO: this code should use ewk_geolocation_permission_request_suspend and check its behaviour.
//Results should be reported using one of utc_ macros */

  ewk_geolocation_permission_request_suspend(NULL);
}
