// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_view_back_forward_list_get : public utc_blink_ewk_base
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

const char* const utc_blink_ewk_view_back_forward_list_get::TEST_URL1 = "ewk_history/page1.html";
const char* const utc_blink_ewk_view_back_forward_list_get::TEST_URL2 = "ewk_history/page2.html";
const char* const utc_blink_ewk_view_back_forward_list_get::TEST_URL3 = "ewk_history/page3.html";

TEST_F(utc_blink_ewk_view_back_forward_list_get, POS_TEST)
{
  // load 3 pages to get some interesting history
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), GetResourceUrl(TEST_URL1).c_str()));
  ASSERT_EQ(Success, EventLoopStart());
  std::string expectedTitle1(ewk_view_title_get(GetEwkWebView()));
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), GetResourceUrl(TEST_URL2).c_str()));
  ASSERT_EQ(Success, EventLoopStart());
  std::string expectedTitle2(ewk_view_title_get(GetEwkWebView()));
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), GetResourceUrl(TEST_URL3).c_str()));
  ASSERT_EQ(Success, EventLoopStart());
  std::string expectedTitle3(ewk_view_title_get(GetEwkWebView()));

  // get back-forward list
  Ewk_Back_Forward_List *list = ewk_view_back_forward_list_get(GetEwkWebView());
  ASSERT_TRUE(list);

  // check list count
  ASSERT_EQ(3, ewk_back_forward_list_count(list));

  // get current item and check URL, original URL and title
  Ewk_Back_Forward_List_Item *item = ewk_back_forward_list_current_item_get(list);

  ASSERT_STREQ(GetResourceUrl(TEST_URL3).c_str(), ewk_back_forward_list_item_url_get(item));
  ASSERT_STREQ(GetResourceUrl(TEST_URL3).c_str(), ewk_back_forward_list_item_original_url_get(item));
  ASSERT_STREQ(expectedTitle3.c_str(), ewk_back_forward_list_item_title_get(item));

  // go back
  ASSERT_EQ(EINA_TRUE, ewk_view_back(GetEwkWebView()));
  ASSERT_EQ(Success, EventLoopStart());

  // check list count
  ASSERT_EQ(3, ewk_back_forward_list_count(list));

  // get current item and check URL, original URL and title
  item = ewk_back_forward_list_current_item_get(list);

  ASSERT_STREQ(GetResourceUrl(TEST_URL2).c_str(), ewk_back_forward_list_item_url_get(item));
  ASSERT_STREQ(GetResourceUrl(TEST_URL2).c_str(), ewk_back_forward_list_item_original_url_get(item));
  ASSERT_STREQ(expectedTitle2.c_str(), ewk_back_forward_list_item_title_get(item));

  // go back
  ASSERT_EQ(EINA_TRUE, ewk_view_back(GetEwkWebView()));
  ASSERT_EQ(Success, EventLoopStart());

  // check list count
  ASSERT_EQ(3, ewk_back_forward_list_count(list));

  // get current item and check URL, original URL and title
  item = ewk_back_forward_list_current_item_get(list);

  ASSERT_STREQ(GetResourceUrl(TEST_URL1).c_str(), ewk_back_forward_list_item_url_get(item));
  ASSERT_STREQ(GetResourceUrl(TEST_URL1).c_str(), ewk_back_forward_list_item_original_url_get(item));
  ASSERT_STREQ(expectedTitle1.c_str(), ewk_back_forward_list_item_title_get(item));

  // load page so the list should be changed
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), GetResourceUrl(TEST_URL3).c_str()));
  ASSERT_EQ(Success, EventLoopStart());

  // check list count
  ASSERT_EQ(2, ewk_back_forward_list_count(list));

  // get current item and check URL, original URL and title
  item = ewk_back_forward_list_current_item_get(list);

  ASSERT_STREQ(GetResourceUrl(TEST_URL3).c_str(), ewk_back_forward_list_item_url_get(item));
  ASSERT_STREQ(GetResourceUrl(TEST_URL3).c_str(), ewk_back_forward_list_item_original_url_get(item));
  ASSERT_STREQ(expectedTitle3.c_str(), ewk_back_forward_list_item_title_get(item));
}

TEST_F(utc_blink_ewk_view_back_forward_list_get, EMPTY_TEST)
{
  Ewk_Back_Forward_List *list = ewk_view_back_forward_list_get(GetEwkWebView());
  ASSERT_TRUE(list);
  ASSERT_EQ(0, ewk_back_forward_list_count(list));
}

TEST_F(utc_blink_ewk_view_back_forward_list_get, NULL_TEST)
{
  ASSERT_FALSE(ewk_view_back_forward_list_get(NULL));
}
