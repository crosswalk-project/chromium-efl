// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
