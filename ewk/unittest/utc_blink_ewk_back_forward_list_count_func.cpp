// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_back_forward_list_count : public utc_blink_ewk_base
{
protected:
  void LoadFinished(Evas_Object *) {
    EventLoopStop(Success);
  }

protected:
  static const char* const TEST_URL1;
  static const char* const TEST_URL2;
  static const char* const TEST_URL3;
};

const char* const utc_blink_ewk_back_forward_list_count::TEST_URL1 = "ewk_history/page1.html";
const char* const utc_blink_ewk_back_forward_list_count::TEST_URL2 = "ewk_history/page2.html";
const char* const utc_blink_ewk_back_forward_list_count::TEST_URL3 = "ewk_history/page3.html";

TEST_F(utc_blink_ewk_back_forward_list_count, POS_TEST)
{
  // get back-forward list
  Ewk_Back_Forward_List *list = ewk_view_back_forward_list_get(GetEwkWebView());
  ASSERT_TRUE(list);

  // it should be empty
  ASSERT_EQ(0, ewk_back_forward_list_count(list));

  // load page
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), GetResourceUrl(TEST_URL1).c_str()));
  ASSERT_EQ(Success, EventLoopStart());

  // check count
  ASSERT_EQ(1, ewk_back_forward_list_count(list));

  // load page
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), GetResourceUrl(TEST_URL2).c_str()));
  ASSERT_EQ(Success, EventLoopStart());

  // check count
  ASSERT_EQ(2, ewk_back_forward_list_count(list));

  // load page
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), GetResourceUrl(TEST_URL3).c_str()));
  ASSERT_EQ(Success, EventLoopStart());

  // check count
  ASSERT_EQ(3, ewk_back_forward_list_count(list));

  // go back
  ASSERT_EQ(EINA_TRUE, ewk_view_back(GetEwkWebView()));
  ASSERT_EQ(Success, EventLoopStart());

  // check count
  ASSERT_EQ(3, ewk_back_forward_list_count(list));

  // go back
  ASSERT_EQ(EINA_TRUE, ewk_view_back(GetEwkWebView()));
  ASSERT_EQ(Success, EventLoopStart());

  // check count
  ASSERT_EQ(3, ewk_back_forward_list_count(list));

  // load page so the list should change and rebuild internal cache
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), GetResourceUrl(TEST_URL3).c_str()));
  ASSERT_EQ(Success, EventLoopStart());

  // check count
  ASSERT_EQ(2, ewk_back_forward_list_count(list));
}

TEST_F(utc_blink_ewk_back_forward_list_count, EMPTY_TEST)
{
  Ewk_Back_Forward_List *list = ewk_view_back_forward_list_get(GetEwkWebView());
  ASSERT_TRUE(list);
  ASSERT_EQ(0, ewk_back_forward_list_count(list));
}

TEST_F(utc_blink_ewk_back_forward_list_count, NULL_TEST)
{
  ASSERT_EQ(0, ewk_back_forward_list_count(NULL));
}
