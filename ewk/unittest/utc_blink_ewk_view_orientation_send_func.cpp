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

class utc_blink_ewk_view_orientation_send : public utc_blink_ewk_base
{
protected:
  utc_blink_ewk_view_orientation_send()
    : utc_blink_ewk_base()
    , g_orientation(-1)
  {
  }

  void LoadFinished(Evas_Object* webview)
  {
    EventLoopStop(utc_blink_ewk_base::Success);
  }

  static Eina_Bool alert_callback(Evas_Object* webview ,const char* alert_text, void* user_data)
  {
    if (user_data) {
      utc_blink_ewk_view_orientation_send* owner = NULL;
      OwnerFromVoid(user_data, &owner);

      long int orientation = strtol(alert_text, NULL, 10);
      // TODO: check errno

      fprintf(stderr,"[alert_callback] :: %ld\n", orientation);
      switch (orientation) {
      case -90:
      case 0:
      case 90:
      case 180:
        owner->g_orientation = orientation;
        break;
      }

      owner->EventLoopStop(Success);
      utc_message("[alert_callback] :: going out");
    }

    // behave like dialog was closed
    return EINA_FALSE;
  }

  /* Startup function */
  void PostSetUp()
  {
    ewk_view_javascript_alert_callback_set(GetEwkWebView(), alert_callback, this);

    // All TCs require same page to load
    std::string full_path = GetResourceUrl("ewk_view/orientation_test.html");
    ASSERT_FALSE(full_path.empty());
    ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), full_path.c_str()));
    ASSERT_EQ(Success, EventLoopStart());
  }

protected:
  long int g_orientation;
};

/**
 * @brief Checking whether orientation is set to 90 properly.
 */
TEST_F(utc_blink_ewk_view_orientation_send, TEST_90DG)
{
  utc_message("[utc_blink_ewk_view_orientation_send TEST_90DG] :: ewk_view_orientation_send");
  ewk_view_orientation_send(GetEwkWebView(), 90);

  ASSERT_EQ(Success, EventLoopStart());
  ASSERT_EQ(g_orientation, 90);
}

/**
 * @brief Checking whether orientation is set to 180 properly.
 */
TEST_F(utc_blink_ewk_view_orientation_send, TEST_180DG)
{
  utc_message("[utc_blink_ewk_view_orientation_send TEST_180DG] :: ewk_view_orientation_send");
  ewk_view_orientation_send(GetEwkWebView(), 180);

  ASSERT_EQ(Success, EventLoopStart());
  ASSERT_EQ(g_orientation, 180);
}

/**
 * @brief Checking whether orientation is set to 0 properly.
 */
TEST_F(utc_blink_ewk_view_orientation_send, TEST_0DG)
{
  utc_message("[utc_blink_ewk_view_orientation_send TEST_0DG] :: ewk_view_orientation_send");
  ewk_view_orientation_send(GetEwkWebView(), 0);

  ASSERT_EQ(Success, EventLoopStart());
  ASSERT_EQ(g_orientation, 0);
}

/**
 * @brief Checking whether orientation is set to -90 properly.
 */
TEST_F(utc_blink_ewk_view_orientation_send, TEST_MINUS90DG)
{
  utc_message("[utc_blink_ewk_view_orientation_send TEST_MINUS90DG] :: ewk_view_orientation_send");
  ewk_view_orientation_send(GetEwkWebView(), -90);

  ASSERT_EQ(Success, EventLoopStart());
  ASSERT_EQ(g_orientation, -90);
}

/**
 * @brief Checking whether function works properly in case of NULL of a webview.
 */
TEST_F(utc_blink_ewk_view_orientation_send, INVALID_ARGS)
{
  ewk_view_orientation_send(NULL, EWK_SCREEN_ORIENTATION_PORTRAIT_PRIMARY);
  EXPECT_EQ(Failure, EventLoopStart(5.0));

  ewk_view_orientation_send(GetEwkWebView(), 22);
  EXPECT_EQ(Failure, EventLoopStart(5.0));
}
