// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

#define URL  ("ewk_view/orientation_test.html")

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
    g_orientation = atol(ewk_view_title_get(GetEwkWebView()));
    EventLoopStop(utc_blink_ewk_base::Success);
  }


  void ConsoleMessage(Evas_Object* webview, const Ewk_Console_Message* msg)
  {
    utc_blink_ewk_base::ConsoleMessage(webview, msg);

    const char* message_text = ewk_console_message_text_get(msg);
    long int orientation = atol(message_text);

    switch (orientation) {
    case -90:
    case 0:
    case 90:
    case 180:
      g_orientation = orientation;
      break;
    }

    EventLoopStop(utc_blink_ewk_base::Success);
  }

protected:
  long int g_orientation;
};

/**
 * @brief Checking whether orientation is set to 90 properly.
 */
TEST_F(utc_blink_ewk_view_orientation_send, TEST_90DG)
{
  std::string full_path = GetResourceUrl(URL);
  ASSERT_FALSE(full_path.empty());
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), full_path.c_str()));
  ASSERT_EQ(Success, EventLoopStart());

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
  std::string full_path = GetResourceUrl(URL);
  ASSERT_FALSE(full_path.empty());
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), full_path.c_str()));
  ASSERT_EQ(Success, EventLoopStart());

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
  std::string full_path = GetResourceUrl(URL);
  ASSERT_FALSE(full_path.empty());
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), full_path.c_str()));
  ASSERT_EQ(Success, EventLoopStart());

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
  std::string full_path = GetResourceUrl(URL);
  ASSERT_FALSE(full_path.empty());
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), full_path.c_str()));
  ASSERT_EQ(Success, EventLoopStart());

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
  std::string full_path = GetResourceUrl(URL);
  ASSERT_FALSE(full_path.empty());
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), full_path.c_str()));
  ASSERT_EQ(Success, EventLoopStart());

  ewk_view_orientation_send(NULL, EWK_SCREEN_ORIENTATION_PORTRAIT_PRIMARY);
  EXPECT_EQ(Timeout, EventLoopStart(2.0));

  ewk_view_orientation_send(GetEwkWebView(), 22);
  EXPECT_EQ(Timeout, EventLoopStart(2.0));
}

/**
 * @brief Checking whether function works properly in case of no render widget
 *        host view object, that is if information about the orientation set
 *        is not lost when function is called before setting the url.
 */
TEST_F(utc_blink_ewk_view_orientation_send, BEFOREURLSET)
{
  std::string full_path = GetResourceUrl(URL);
  ASSERT_FALSE(full_path.empty());

  ewk_view_orientation_send(GetEwkWebView(), 90);

  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), full_path.c_str()));
  EXPECT_EQ(Success, EventLoopStart());
  ASSERT_EQ(g_orientation, 90);
}