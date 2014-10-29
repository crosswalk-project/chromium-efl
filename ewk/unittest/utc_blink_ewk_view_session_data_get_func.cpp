// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_view_session_data_get_func : public utc_blink_ewk_base
{
protected:
  std::string firstPage;
  const char *sessionData;

  void PreSetUp()
  {
    firstPage = GetResourceUrl("common/sample.html");
    sessionData = NULL;
  }

  void LoadFinished(Evas_Object*)
  {
    EventLoopStop(Success);
  }

  void PostTearDown()
  {
    if (sessionData)
      free(const_cast<char *>(sessionData));
  }
};


/**
 * @brief Tests if returns valid webview when called with correct Evas object.
 */
TEST_F(utc_blink_ewk_view_session_data_get_func, POS_TEST)
{
  ewk_view_url_set(GetEwkWebView(), firstPage.c_str());
  EventLoopStart();

  unsigned length = 0;
  ewk_view_session_data_get(GetEwkWebView(), &sessionData, &length);

  ASSERT_TRUE(sessionData);
  ASSERT_NE(length, 0);
}

/**
 * @brief Tests if returns NULL when called with NULL webview object.
 */
TEST_F(utc_blink_ewk_view_session_data_get_func, NEG_TEST_NULL_WEBVIEW)
{
  ewk_view_url_set(GetEwkWebView(), firstPage.c_str());
  EventLoopStart();

  const char *data;
  unsigned length;
  ewk_view_session_data_get(NULL, &data, &length);

  ASSERT_FALSE(data);
  ASSERT_EQ(length, 0);
}

/**
 * @brief Tests if returns NULL when called with NULL data.
 */
TEST_F(utc_blink_ewk_view_session_data_get_func, NEG_TEST_NULL_DATA)
{
  ewk_view_url_set(GetEwkWebView(), firstPage.c_str());
  EventLoopStart();

  unsigned length;
  // Should not crash
  ewk_view_session_data_get(GetEwkWebView(), NULL, &length);
}

/**
 * @brief Tests if returns NULL when called with NULL length.
 */
TEST_F(utc_blink_ewk_view_session_data_get_func, NEG_TEST_NULL_LENGTH)
{
  ewk_view_url_set(GetEwkWebView(), firstPage.c_str());
  EventLoopStart();

  const char *data;
  // Should not crash
  ewk_view_session_data_get(GetEwkWebView(), &data, NULL);
}
