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
