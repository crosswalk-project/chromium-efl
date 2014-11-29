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

#define SAMPLE_HTML_FILE                 ("/common/sample_js_geolocation.html")

class utc_blink_ewk_geolocation_permission_request_set : public utc_blink_ewk_base {

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
    utc_blink_ewk_geolocation_permission_request_set *owner = static_cast<utc_blink_ewk_geolocation_permission_request_set*>(data);

    Ewk_Geolocation_Permission_Request* permission_request = (Ewk_Geolocation_Permission_Request*)event_info;
    if (permission_request && ewk_geolocation_permission_request_set(permission_request, EINA_TRUE)) {

       owner->EventLoopStop(utc_blink_ewk_base::Success);
    }
  }
};

/**
 * @brief Positive test whether request of permission is allowed when web page requests geolocation permission for frame.
 */
TEST_F(utc_blink_ewk_geolocation_permission_request_set, POS_TEST)
{
  Eina_Bool result = ewk_view_url_set(GetEwkWebView(), GetResourceUrl(SAMPLE_HTML_FILE).c_str());
  if (!result)
    FAIL();

  utc_blink_ewk_base::MainLoopResult main_result = EventLoopStart();
  if (main_result != utc_blink_ewk_base::Success)
    FAIL();

//TODO: this code should use ewk_geolocation_permission_request_set and check its behaviour.
//Results should be reported using one of utc_ macros */
}

/**
 * @brief Negative test whether request of permission is allowed when web page requests geolocation permission for frame.
 */
TEST_F(utc_blink_ewk_geolocation_permission_request_set, NEG_TEST)
{
  Eina_Bool test_result = EINA_FALSE;
  if( ewk_geolocation_permission_request_set(NULL, EINA_TRUE) ) {

    test_result = EINA_TRUE;
  }

//TODO: this code should use ewk_geolocation_permission_request_set and check its behaviour.
//Results should be reported using one of utc_ macros */
}
