// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/* Define those macros _before_ you include the utc_blink_ewk.h header file. */

#include "utc_blink_ewk_base.h"

#define SAMPLE_HTML_FILE                 ("/common/sample_js_geolocation.html")

class utc_blink_ewk_geolocation_permission_request_origin_get : public utc_blink_ewk_base {

protected:

  void PostSetUp()
  {
    evas_object_smart_callback_add(GetEwkWebView(),"geolocation,permission,request", request_geolocation_permisson, this);
  }

  void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(),"geolocation,permission,request", request_geolocation_permisson);
  }

  void LoadFinished(Evas_Object* webview) {

    EventLoopStop(utc_blink_ewk_base::Failure);
  }

  static void request_geolocation_permisson(void* data, Evas_Object* webview, void* event_info)
  {
    utc_message("[request_geolocation_permisson] :: \n");
    utc_blink_ewk_geolocation_permission_request_origin_get *owner = static_cast<utc_blink_ewk_geolocation_permission_request_origin_get*>(data);

    Ewk_Geolocation_Permission_Request* permission_request = (Ewk_Geolocation_Permission_Request*)event_info;
    if (permission_request && ewk_geolocation_permission_request_origin_get(permission_request)) {

        owner->EventLoopStop(utc_blink_ewk_base::Success);
    }
  }
};

/**
 * @brief Positive test whether Ewk_Geolocation_Permission_Request has the Ewk_Security_Origin value when web page requests geolocation permission for frame.
 */
TEST_F(utc_blink_ewk_geolocation_permission_request_origin_get, POS_TEST)
{
  Eina_Bool result = ewk_view_url_set(GetEwkWebView(), GetResourceUrl(SAMPLE_HTML_FILE).c_str());
  if (!result)
    FAIL();

  utc_blink_ewk_base::MainLoopResult main_result = EventLoopStart();
  if (main_result != utc_blink_ewk_base::Success)
    FAIL();

//TODO: this code should use ewk_geolocation_permission_request_origin_get and check its behaviour.
//Results should be reported using one of utc_ macros
}

/**
 * @brief Negative test whether Ewk_Geolocation_Permission_Data has the Ewk_Geolocation_Permission_Request value when web page requests geolocation permission for frame.
 */
TEST_F(utc_blink_ewk_geolocation_permission_request_origin_get, NEG_TEST)
{
  Eina_Bool test_result = EINA_FALSE;
  if( ewk_geolocation_permission_request_origin_get(NULL) ) {

    test_result = EINA_TRUE;
  }
  utc_check_ne(test_result, EINA_TRUE);

//TODO: this code should use ewk_geolocation_permission_request_origin_get and check its behaviour.
//Results should be reported using one of utc_ macros
}
