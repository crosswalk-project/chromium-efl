// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"


class utc_blink_ewk_view_forward : public utc_blink_ewk_base {
protected:

  void LoadFinished(Evas_Object* webview)
  {
    EventLoopStop(utc_blink_ewk_base::Success);
  }

protected:
  static const char * const SAMPLE_HTML_FILE;
  static const char * const SAMPLE_1_HTML_FILE;
};

const char * const utc_blink_ewk_view_forward::SAMPLE_HTML_FILE = "/common/sample.html";
const char * const utc_blink_ewk_view_forward::SAMPLE_1_HTML_FILE = "/common/sample_1.html";

/**
* @brief Check whether it is possible to go forward when two pages are loaded and back is done
*/
TEST_F(utc_blink_ewk_view_forward, POS_TEST)
{
  if (!ewk_view_url_set(GetEwkWebView(), GetResourceUrl(SAMPLE_HTML_FILE).c_str())) {
    FAIL();
  }
  if (utc_blink_ewk_base::Success != EventLoopStart()) {
    FAIL();
  }

  if (!ewk_view_url_set(GetEwkWebView(), GetResourceUrl(SAMPLE_1_HTML_FILE).c_str())) {
    FAIL();
  }
  if (utc_blink_ewk_base::Success != EventLoopStart()) {
    FAIL();
  }

  Eina_Bool backResult = ewk_view_back(GetEwkWebView());
  if (utc_blink_ewk_base::Success != EventLoopStart()) {
    FAIL();
  }

  const char* currentUrl = ewk_view_url_get(GetEwkWebView());
  EXPECT_STREQ(currentUrl, GetResourceUrl(SAMPLE_HTML_FILE).c_str());

  Eina_Bool forwardResult = ewk_view_forward(GetEwkWebView());
  if (utc_blink_ewk_base::Success != EventLoopStart()) {
    FAIL();
  }

  EXPECT_EQ(forwardResult, EINA_TRUE);
}

/**
* @brief Checking whether function works properly in case of NULL of a webview.
*/
TEST_F(utc_blink_ewk_view_forward, NEG_TEST)
{
  Eina_Bool result = ewk_view_forward(NULL);
  EXPECT_EQ(result, EINA_FALSE);
}
