// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_view_scale_get : public utc_blink_ewk_base
{
 protected:

  /* Callback for load finished */
  void LoadFinished(Evas_Object* webview)
  {
    EventLoopStop(Success);
  }

  /* Function called when timer expires */
  static Eina_Bool mainLoopQuit(void* data)
  {
    if(data)
      static_cast<utc_blink_ewk_view_scale_get*>(data)->EventLoopStop(Success);
    return ECORE_CALLBACK_DONE;
  }

protected:
  static const char*const sample;
};

const char*const utc_blink_ewk_view_scale_get::sample="/common/sample.html";

/**
  * @brief Positive test case of ewk_view_scale_get(). Page is loaded, APIs for setting scale and retrieving its value are called.
  */
TEST_F(utc_blink_ewk_view_scale_get, POS_TEST)
{
  if(!ewk_view_url_set(GetEwkWebView(),GetResourceUrl(sample).c_str()))
    utc_fail();
  if(Success!=EventLoopStart())
    utc_fail();

  const double scaleFactor = 1.0;
  const Evas_Coord centerX = 5;
  const Evas_Coord centerY = 5;

  Eina_Bool result = ewk_view_scale_set(GetEwkWebView(), scaleFactor, centerX, centerY);
  ecore_timer_add(0.5f, mainLoopQuit, this);
  if(Success!=EventLoopStart())
    utc_fail();

  const double getScaleFactor = ewk_view_scale_get(GetEwkWebView());

  if (scaleFactor != getScaleFactor)
    result = EINA_FALSE;

  utc_check_eq(result, EINA_TRUE);
}

/**
  * @brief Checking whether function works properly in case of NULL of a webview.
  */
TEST_F(utc_blink_ewk_view_scale_get, NEG_TEST)
{
  const double getScaleFactor = ewk_view_scale_get(NULL);
  utc_check_eq(getScaleFactor, -1);
}
