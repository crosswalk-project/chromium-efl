/*
 * chromium EFL
 *
 * Copyright (c) 2012 Samsung Electronics Co., Ltd.
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

class utc_blink_ewk_view_resume : public utc_blink_ewk_base
{
protected:
  enum RunPhase {
    PageLoadStart,
    PageSuspended,
    PageResumed
  };

protected:
  utc_blink_ewk_view_resume()
    : phase(PageLoadStart)
  {
  }

  /* Callback for quitting main loop */
  void LoadProgress(Evas_Object* webview, double pr)
  {
    // Page started to load, after that we want to suspend right after
    if (phase == PageLoadStart) {
      phase = PageSuspended;
      EventLoopStop(Success);
    }
  }

  bool TimeOut()
  {
    if (phase == PageSuspended) {
      phase = PageResumed;
      EventLoopStop(Success);
      return true;
    }

    return false;
  }

  void LoadFinished(Evas_Object* webview)
  {
    if (phase == PageResumed) {
      EventLoopStop(Success);
    } else {
      EventLoopStop(Failure);
    }
  }

protected:
  RunPhase phase;
};

/**
 * @brief Positive test case of ewk_view_resume()
 */
TEST_F(utc_blink_ewk_view_resume, POS_TEST)
{
  if (!ewk_view_url_set(GetEwkWebView(), "http://www.google.pl"))
    FAIL();

  if (Success != EventLoopStart())
    FAIL();

  utc_message("Suspend");
  ewk_view_suspend(GetEwkWebView());

  if (Success != EventLoopStart(5.0)) // Wait few seconds to see if it realy was suspended
    FAIL();

  utc_message("Resume");
  ewk_view_resume(GetEwkWebView());

  if (Success != EventLoopStart())
    FAIL();

  evas_object_show(GetEwkWebView());
  evas_object_show(GetEwkWindow());
}

/**
 * @brief Negative test case of ewk_view_resume()
 */
TEST_F(utc_blink_ewk_view_resume, NEG_TEST)
{
  // This test is pointless. ewk_view_resume has no return value so we have nothing to test here.
  ewk_view_resume(NULL);
  evas_object_show(GetEwkWebView());
  evas_object_show(GetEwkWindow());
}
