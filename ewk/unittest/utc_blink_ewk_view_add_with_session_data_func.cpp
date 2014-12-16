// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_view_add_with_session_data_func : public utc_blink_ewk_base
{
protected:
  std::string firstPage;
  std::string testUrl;
  std::string lastPage;

  const char *sessionData;
  Evas_Object *otherWebview;

  void PreSetUp()
  {
    firstPage = GetResourceUrl("common/sample.html");
    testUrl   = GetResourceUrl("common/sample_1.html");
    lastPage  = GetResourceUrl("common/sample_2.html");

    otherWebview = NULL;
    sessionData = NULL;
  }

  void LoadFinished(Evas_Object*)
  {
    EventLoopStop(Success);
  }

  void PostTearDown()
  {
    if (otherWebview)
      evas_object_del(otherWebview);
    if (sessionData)
    free(const_cast<char *>(sessionData));
  }
};


/**
 * @brief Tests if returns valid webview when called with correct Evas object.
 */
TEST_F(utc_blink_ewk_view_add_with_session_data_func, POS_TEST)
{
  ewk_view_url_set(GetEwkWebView(), firstPage.c_str());
  ASSERT_EQ(EventLoopStart(), Success);
  ewk_view_url_set(GetEwkWebView(), testUrl.c_str());
  ASSERT_EQ(EventLoopStart(), Success);
  ewk_view_url_set(GetEwkWebView(), lastPage.c_str());
  ASSERT_EQ(EventLoopStart(), Success);
  ewk_view_back(GetEwkWebView());
  ASSERT_EQ(EventLoopStart(), Success);

  ASSERT_TRUE(ewk_view_forward_possible(GetEwkWebView())) << "before";
  ASSERT_TRUE(ewk_view_back_possible(GetEwkWebView())) << "before";

  unsigned length = 0;
  ewk_view_session_data_get(GetEwkWebView(), &sessionData, &length);

  ASSERT_TRUE(sessionData);
  ASSERT_NE(length, 0);

  otherWebview = ewk_view_add_with_session_data(GetEwkEvas(), sessionData, length);
  ASSERT_TRUE(otherWebview);

  ASSERT_STREQ(testUrl.c_str(), ewk_view_url_get(otherWebview));
  ASSERT_TRUE(ewk_view_forward_possible(otherWebview)) << "after";
  ASSERT_TRUE(ewk_view_back_possible(otherWebview)) << "after";
}

/**
 * @brief Tests if returns NULL when called with NULL Evas object.
 */
TEST_F(utc_blink_ewk_view_add_with_session_data_func, NEG_TEST_NULL_EVAS)
{
  ewk_view_url_set(GetEwkWebView(), firstPage.c_str());
  ASSERT_EQ(EventLoopStart(), Success);

  const char *data;
  unsigned length;
  ewk_view_session_data_get(GetEwkWebView(), &data, &length);

  ewk_view_add_with_session_data(NULL, data, length);
}

/**
 * @brief Tests if returns NULL when called with NULL data.
 */
TEST_F(utc_blink_ewk_view_add_with_session_data_func, POS_TEST_NULL_DATA)
{
  const char *data = NULL;
  unsigned length = 0;
  ASSERT_TRUE(ewk_view_add_with_session_data(GetEwkEvas(), data, length) != NULL);
}

/**
 * @brief Tests if returns NULL when called with invalid data.
 */
TEST_F(utc_blink_ewk_view_add_with_session_data_func, NEG_TEST_INVALID_DATA)
{
  const char data[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  unsigned length = sizeof(data) / sizeof(data[0]);

  ASSERT_FALSE(ewk_view_add_with_session_data(GetEwkEvas(), data, length));
}
