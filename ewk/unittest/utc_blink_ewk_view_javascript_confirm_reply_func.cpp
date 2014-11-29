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
