// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

#define URL  ("http://google.com")

class utc_blink_cb_load_progress_finished : public utc_blink_ewk_base
{
  public:
    void LoadProgressFinished(Evas_Object* webview)
    {
        EventLoopStop(utc_blink_ewk_base::Success);
    }
};

/**
 *  * @brief Tests "load,progress,finished" callback
 */
TEST_F(utc_blink_cb_load_progress_finished, callback)
{
  Eina_Bool result = ewk_view_url_set(GetEwkWebView(), URL);

  if (!result)
      utc_fail();

  utc_check_eq(EventLoopStart(), utc_blink_ewk_base::Success);
}
