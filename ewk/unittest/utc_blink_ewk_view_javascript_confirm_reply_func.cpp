// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_view_javascript_confirm_reply : public utc_blink_ewk_base
{
 protected:

  void LoadFinished(Evas_Object* webview)
  {
    EventLoopStop(utc_blink_ewk_base::Success);
  }

  /* Callback for script confirm */
  static Eina_Bool confirmCallback(Evas_Object* webview, const char* message, void* data)
  {
    Eina_Bool result = EINA_TRUE;
    ewk_view_javascript_confirm_reply(webview, result);
  }
};

/**
 * @brief Positive  test case of ewk_view_javascript_confirm_reply()
 * ewk_view_javascript_confirm_reply() should be performed without any problem.
 */
TEST_F(utc_blink_ewk_view_javascript_confirm_reply, POS_TEST)
{
  ewk_view_javascript_confirm_callback_set(GetEwkWebView(), confirmCallback, this);

  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), GetResourceUrl("ewk_view/confirm_test.html").c_str()));
  ASSERT_EQ(Success, EventLoopStart());

  const char* title = ewk_view_title_get(GetEwkWebView());
  const char* expected = "You pressed OK!";

  EXPECT_STREQ(expected, title);
}

/**
* @brief Checking whether function works properly in case of NULL of a webview.
*/
TEST_F(utc_blink_ewk_view_javascript_confirm_reply, NEG_TEST)
{
  ewk_view_javascript_confirm_reply(NULL, EINA_FALSE);
}
