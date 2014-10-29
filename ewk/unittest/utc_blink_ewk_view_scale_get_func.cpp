// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"
#include <limits>

class utc_blink_ewk_view_scale_get : public utc_blink_ewk_base
{
 protected:
  utc_blink_ewk_view_scale_get() : precision(std::numeric_limits<double>::epsilon()) { }

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
  const double precision;
};

const char*const utc_blink_ewk_view_scale_get::sample="/common/sample.html";

/**
  * @brief Positive test case of ewk_view_scale_get(). Page is loaded, APIs for setting scale and retrieving its value are called.
  */
TEST_F(utc_blink_ewk_view_scale_get, POS_TEST)
{
  ASSERT_TRUE(ewk_view_url_set(GetEwkWebView(), GetResourceUrl(sample).c_str()));
  ASSERT_EQ(Success, EventLoopStart());

  double minScale = 0, maxScale = 0;
  ewk_view_scale_range_get(GetEwkWebView(), &minScale, &maxScale);
  ASSERT_GT(abs(maxScale - minScale), precision);

  const double scaleFactor = minScale + (maxScale - minScale)/2;
  const Evas_Coord centerX = 5;
  const Evas_Coord centerY = 5;

  ASSERT_TRUE(ewk_view_scale_set(GetEwkWebView(), scaleFactor, centerX, centerY));
  double currentScale = ewk_view_scale_get(GetEwkWebView());
  ASSERT_LT(abs(scaleFactor - currentScale), precision);

  ecore_timer_add(0.5f, mainLoopQuit, this);
  ASSERT_EQ(Success, EventLoopStart());

  currentScale = ewk_view_scale_get(GetEwkWebView());
  ASSERT_LT(abs(scaleFactor - currentScale), precision);

  ASSERT_TRUE(ewk_view_scale_set(GetEwkWebView(), maxScale + 1, centerX, centerY));
  currentScale = ewk_view_scale_get(GetEwkWebView());
  ASSERT_LT(abs(maxScale - currentScale), precision);

  // Now scale is set to max. Set overscale once again to make sure getter return correct value
  ASSERT_TRUE(ewk_view_scale_set(GetEwkWebView(), maxScale + 1, centerX, centerY));
  currentScale = ewk_view_scale_get(GetEwkWebView());
  ASSERT_LT(abs(maxScale - currentScale), precision);

  ASSERT_TRUE(ewk_view_scale_set(GetEwkWebView(), minScale - 1, centerX, centerY));
  currentScale = ewk_view_scale_get(GetEwkWebView());
  ASSERT_LT(abs(minScale - currentScale), precision);
}

/**
  * @brief Checking whether function works properly in case of NULL of a webview.
  */
TEST_F(utc_blink_ewk_view_scale_get, NEG_TEST)
{
  const double getScaleFactor = ewk_view_scale_get(NULL);
  ASSERT_LT(abs(getScaleFactor + 1), precision);
}
