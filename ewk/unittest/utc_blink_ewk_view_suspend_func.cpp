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

class utc_blink_ewk_view_suspend : public utc_blink_ewk_base
{
protected:
  utc_blink_ewk_view_suspend(): after_progress(false)
  {
  }

  void LoadProgress(Evas_Object* webview, double progress)
  {
    if (!after_progress) {
      // Page started to load, after that we want to suspend it
      after_progress = true;
      EventLoopStop(utc_blink_ewk_base::Success);
    }
  }

  bool TimeOut()
  {
    // If we reached timeout then load,finished (and load,error) was not called
    // we expect that - this indicates that suspend did work
    EventLoopStop(utc_blink_ewk_base::Success);
    return true;
  }

  void LoadFinished(Evas_Object* webview)
  {
    // Load finished should never be called after suspend
    EventLoopStop(utc_blink_ewk_base::Failure);
  }

protected:
  bool after_progress;
};

/**
 * @brief Positive test case of ewk_view_suspend()
 */
TEST_F(utc_blink_ewk_view_suspend, POS_TEST)
{
  if(!ewk_view_url_set(GetEwkWebView(), "http://www.google.pl"))
    FAIL();

  utc_blink_ewk_base::MainLoopResult main_result = EventLoopStart();
  if (main_result != utc_blink_ewk_base::Success)
    FAIL();

  ewk_view_suspend(GetEwkWebView());

  main_result = EventLoopStart(5.0);
  if (main_result != utc_blink_ewk_base::Success)
    FAIL();

  evas_object_show(GetEwkWebView());
  evas_object_show(GetEwkWindow());
}

/**
 * @brief Negative test case of ewk_view_suspend()
 */
TEST_F(utc_blink_ewk_view_suspend, NEG_TEST)
{
  ewk_view_suspend(NULL);
  evas_object_show(GetEwkWebView());
  evas_object_show(GetEwkWindow());
}
