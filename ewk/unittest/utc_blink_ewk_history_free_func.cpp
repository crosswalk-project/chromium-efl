// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_history_free : public utc_blink_ewk_base
{
protected:
  void LoadFinished(Evas_Object *) {
    EventLoopStop(Success);
  }

protected:
  static const char* const TEST_URL;
};

const char* const utc_blink_ewk_history_free::TEST_URL = "ewk_history/page1.html";

TEST_F(utc_blink_ewk_history_free, EXISTING_HISTORY)
{
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), GetResourceUrl(TEST_URL).c_str()));
  ASSERT_EQ(Success, EventLoopStart());

  Ewk_History *history = ewk_view_history_get(GetEwkWebView());
  ASSERT_TRUE(history);

  ewk_history_free(history);
}

TEST_F(utc_blink_ewk_history_free, EXISTING_EMPTY_HISTORY)
{
  Ewk_History *history = ewk_view_history_get(GetEwkWebView());
  ASSERT_TRUE(history);

  ewk_history_free(history);
}

TEST_F(utc_blink_ewk_history_free, NULL_HISTORY)
{
  ewk_history_free(NULL);
}
