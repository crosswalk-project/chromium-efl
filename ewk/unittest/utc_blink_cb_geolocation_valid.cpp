// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/* Define those macros _before_ you include the utc_blink_ewk.h header file. */

#include "utc_blink_ewk_base.h"

#define SAMPLE_HTML_FILE                 ("/common/sample_js_geolocation.html")

class utc_blink_cb_geolocation_valid : public utc_blink_ewk_base {

protected:
  static void geolocation_valid_allow_cb(utc_blink_cb_geolocation_valid* owner, Evas_Object* webview, Eina_Bool* allow)
  {
    ASSERT_TRUE(owner);
    if (allow) {
      *allow = EINA_TRUE;
    } else {
      owner->EventLoopStop(Failure);
    }
  }

  static void geolocation_valid_deny_cb(utc_blink_cb_geolocation_valid* owner, Evas_Object* webview, Eina_Bool* allow)
  {
    ASSERT_TRUE(owner);
    if (allow) {
      *allow = EINA_FALSE;
    } else {
      owner->EventLoopStop(Failure);
    }
  }

  static void geolocation_valid_cb(utc_blink_cb_geolocation_valid* owner, Evas_Object* webview, Eina_Bool* allow)
  {
    ASSERT_TRUE(owner);
    owner->EventLoopStop(Success);
  }

  static void request_geolocation_permisson_success(utc_blink_cb_geolocation_valid* owner, Evas_Object* webview, void* event_info)
  {
    ASSERT_TRUE(owner);
    owner->EventLoopStop(Success);
  }

  static void request_geolocation_permisson_fail(utc_blink_cb_geolocation_valid* owner, Evas_Object* webview, void* event_info)
  {
    ASSERT_TRUE(owner);
    owner->EventLoopStop(Failure);
  }

  static void load_finished_fail_cb(utc_blink_cb_geolocation_valid* owner, Evas_Object*, void*)
  {
    ASSERT_TRUE(owner);
    owner->EventLoopStop(Failure);
  }

  static void load_finished_success_cb(utc_blink_cb_geolocation_valid* owner, Evas_Object*, void*)
  {
    ASSERT_TRUE(owner);
    owner->EventLoopStop(Success);
  }
};

/**
 * @brief Test "geolocation,valid" callback
 */
TEST_F(utc_blink_cb_geolocation_valid, callback)
{
  // Simply check if callback is fired, if load,finished comes first then something went wrong
  evas_object_smart_callback_auto gv(GetEwkWebView(), "geolocation,valid", ToSmartCallback(geolocation_valid_cb), this);
  evas_object_smart_callback_auto lf(GetEwkWebView(), "load,finished", ToSmartCallback(load_finished_fail_cb), this);

  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), GetResourceUrl(SAMPLE_HTML_FILE).c_str()));
  ASSERT_EQ(Success, EventLoopStart());
}

/**
 * @brief check if control param of "geolocation,valid" callback works as it suposed to.
 *
 * If we block geolocation module initialization in geolocation,valid callback then no geolocation,permission,request should be triggered.
 */
TEST_F(utc_blink_cb_geolocation_valid, callback_param_to_false)
{
  evas_object_smart_callback_auto gp(GetEwkWebView(), "geolocation,permission,request", ToSmartCallback(request_geolocation_permisson_fail), this);
  evas_object_smart_callback_auto gv(GetEwkWebView(), "geolocation,valid", ToSmartCallback(geolocation_valid_deny_cb), this);
  evas_object_smart_callback_auto lf(GetEwkWebView(), "load,finished", ToSmartCallback(load_finished_success_cb), this);

  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), GetResourceUrl(SAMPLE_HTML_FILE).c_str()));
  ASSERT_EQ(Success, EventLoopStart());
}

/**
 * @brief check if control param of "geolocation,valid" callback works as it suposed to.
 *
 * If we allow geolocation module initialization in geolocation,valid callback then geolocation,permission,request should be triggered.
 */
TEST_F(utc_blink_cb_geolocation_valid, callback_param_to_true)
{
  evas_object_smart_callback_auto gp(GetEwkWebView(), "geolocation,permission,request", ToSmartCallback(request_geolocation_permisson_success), this);
  evas_object_smart_callback_auto gv(GetEwkWebView(), "geolocation,valid", ToSmartCallback(geolocation_valid_allow_cb), this);
  evas_object_smart_callback_auto lf(GetEwkWebView(), "load,finished", ToSmartCallback(load_finished_fail_cb), this);

  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), GetResourceUrl(SAMPLE_HTML_FILE).c_str()));
  ASSERT_EQ(Success, EventLoopStart());
}
