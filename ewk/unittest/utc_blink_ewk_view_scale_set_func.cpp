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

class utc_blink_ewk_view_scale_set : public utc_blink_ewk_base
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
      static_cast<utc_blink_ewk_view_scale_set*>(data)->EventLoopStop(Success);
    return ECORE_CALLBACK_DONE;
  }

protected:
  static const char*const sample;
};

const char*const utc_blink_ewk_view_scale_set::sample="/common/sample.html";

/**
  * @brief Positive test case of ewk_view_scale_set(). Page is loaded, APIs for setting scale and retrieving its value are called.
  */
  TEST_F(utc_blink_ewk_view_scale_set, POS_TEST)
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
  TEST_F(utc_blink_ewk_view_scale_set, NEG_TEST)
  {
    const Eina_Bool result = ewk_view_scale_set(NULL, 1, 0, 0);
    utc_check_eq(result, EINA_FALSE);
  }
