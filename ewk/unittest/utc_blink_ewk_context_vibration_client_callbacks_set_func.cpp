// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_context_vibration_client_callbacks_set : public utc_blink_ewk_base
{
protected:
  static void vibrateCallback(uint64_t vibration_time, void* user_data)
  {
    if(!user_data)
      utc_fail();
    static_cast<utc_blink_ewk_context_vibration_client_callbacks_set*>(user_data)->vibrateCalled = EINA_TRUE;
  }

  static void cancelVibrateCallback(void* user_data)
  {
    if(!user_data)
      utc_fail();
    static_cast<utc_blink_ewk_context_vibration_client_callbacks_set*>(user_data)->cancelCalled = EINA_TRUE;
  }

  static void title_changed(void* data, Evas_Object* webview, void* event)
  {
    if(!data)
      utc_fail();
    static_cast<utc_blink_ewk_context_vibration_client_callbacks_set*>(data)->EventLoopStop(Success);
  }

  /* Startup function */
  void PostSetUp()
  {
    vibrateCalled = EINA_FALSE;
    cancelCalled = EINA_FALSE;
    evas_object_smart_callback_add(GetEwkWebView(), "title,changed", title_changed, this);
  }

  /* Cleanup function */
  void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(), "title,changed", title_changed);
  }

protected:
  static const char*const sample;
  Eina_Bool vibrateCalled;
  Eina_Bool cancelCalled;
};

const char*const utc_blink_ewk_context_vibration_client_callbacks_set::sample="ewk_context_vibration/vibration.html";

/**
 * @brief Tests if vibration callbacks are properly set
 */
TEST_F(utc_blink_ewk_context_vibration_client_callbacks_set, POS_TEST)
{
  ewk_context_vibration_client_callbacks_set(ewk_context_default_get(), vibrateCallback, cancelVibrateCallback, this);

  if(!ewk_view_url_set(GetEwkWebView(), GetResourceUrl(sample).c_str()))
     utc_fail();

  if (Success!=EventLoopStart())
    utc_fail();

  utc_check_eq( vibrateCalled && cancelCalled, EINA_TRUE);
}

/**
 * @brief Tests if ewk_context_vibration_client_callbacks_set crashes on null arguments and set context
 */
TEST_F(utc_blink_ewk_context_vibration_client_callbacks_set, NEG_TEST1)
{
  ewk_context_vibration_client_callbacks_set(ewk_context_default_get(), NULL, NULL, this);
  /* If  NULL argument passing wont give segmentation fault negative test case will pass */
  utc_pass();
}

/**
 * @brief Tests if ewk_context_vibration_client_callbacks_set crashes on null arguments
 */
TEST_F(utc_blink_ewk_context_vibration_client_callbacks_set, NEG_TEST2)
{
  ewk_context_vibration_client_callbacks_set(NULL, NULL, NULL, this);
  /* If  NULL argument passing wont give segmentation fault negative test case will pass */
  utc_pass();
}
