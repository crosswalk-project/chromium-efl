// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/* Define those macros _before_ you include the utc_blink_ewk.h header file. */

#include "utc_blink_ewk_base.h"

#define SAMPLE_HTML_FILE                 ("/common/sample_js_geolocation.html")

class utc_blink_ewk_view_geolocation_permission_callback_set : public utc_blink_ewk_base {

protected:
  utc_blink_ewk_view_geolocation_permission_callback_set()
    : callback_view(NULL)
    , callback_request(NULL)
  {
  }

  static Eina_Bool geolocation_permission_cb(Evas_Object* webview, Ewk_Geolocation_Permission_Request* request, utc_blink_ewk_view_geolocation_permission_callback_set* owner)
  {
    EXPECT_TRUE(owner);

    if (owner) {
      if (request) {
        ewk_geolocation_permission_request_suspend(request);
        owner->callback_request = request;
      }

      owner->callback_view = webview;
      owner->EventLoopStop(Success);

      return EINA_TRUE;
    }

    return EINA_FALSE;
  }

  static void load_finished_fail_cb(utc_blink_ewk_view_geolocation_permission_callback_set* owner, Evas_Object*, void*)
  {
    ASSERT_TRUE(owner);
    owner->EventLoopStop(Failure);
  }

  void PostSetUp()
  {
    evas_object_smart_callback_add(GetEwkWebView(), "load,finished", ToSmartCallback(load_finished_fail_cb), this);
  }

  void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(), "load,finished", ToSmartCallback(load_finished_fail_cb));
  }

protected:
  Evas_Object* callback_view;
  Ewk_Geolocation_Permission_Request* callback_request;
};

/**
 * @brief Test "geolocation,valid" callback
 */
TEST_F(utc_blink_ewk_view_geolocation_permission_callback_set, callback)
{
  ewk_view_geolocation_permission_callback_set(
    GetEwkWebView(),
    reinterpret_cast<Ewk_View_Geolocation_Permission_Callback>(geolocation_permission_cb),
    this);

  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), GetResourceUrl(SAMPLE_HTML_FILE).c_str()));
  ASSERT_EQ(Success, EventLoopStart());

  ASSERT_EQ(callback_view, GetEwkWebView());
  callback_view = NULL;

  ASSERT_TRUE(callback_request);
  ewk_geolocation_permission_request_set(callback_request, EINA_FALSE);
  callback_request = NULL;

  // reset callback
  ewk_view_geolocation_permission_callback_set(GetEwkWebView(), NULL, NULL);
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), GetResourceUrl(SAMPLE_HTML_FILE).c_str()));
  ASSERT_EQ(Failure, EventLoopStart());
}

// No invalid args TC because setter is void
