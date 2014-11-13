// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_view_back_forward_list_item_at_index_get : public utc_blink_ewk_base
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

const char* const utc_blink_ewk_view_back_forward_list_item_at_index_get::TEST_URL1 = "ewk_history/page1.html";
const char* const utc_blink_ewk_view_back_forward_list_item_at_index_get::TEST_URL2 = "ewk_history/page2.html";
const char* const utc_blink_ewk_view_back_forward_list_item_at_index_get::TEST_URL3 = "ewk_history/page3.html";

TEST_F(utc_blink_ewk_view_back_forward_list_item_at_index_get, POS_TEST)
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

  // get current item and check URL, original URL and title
  Ewk_Back_Forward_List_Item *item = ewk_back_forward_list_item_at_index_get(list, 0);

  ASSERT_STREQ(GetResourceUrl(TEST_URL3).c_str(), ewk_back_forward_list_item_url_get(item));
  ASSERT_STREQ(GetResourceUrl(TEST_URL3).c_str(), ewk_back_forward_list_item_original_url_get(item));
  ASSERT_STREQ(expectedTitle3.c_str(), ewk_back_forward_list_item_title_get(item));

  // get item at -1 and check URL, original URL and title
  item = ewk_back_forward_list_item_at_index_get(list, -1);

  ASSERT_STREQ(GetResourceUrl(TEST_URL2).c_str(), ewk_back_forward_list_item_url_get(item));
  ASSERT_STREQ(GetResourceUrl(TEST_URL2).c_str(), ewk_back_forward_list_item_original_url_get(item));
  ASSERT_STREQ(expectedTitle2.c_str(), ewk_back_forward_list_item_title_get(item));

  // get item at -2 and check URL, original URL and title
  item = ewk_back_forward_list_item_at_index_get(list, -2);

  ASSERT_STREQ(GetResourceUrl(TEST_URL1).c_str(), ewk_back_forward_list_item_url_get(item));
  ASSERT_STREQ(GetResourceUrl(TEST_URL1).c_str(), ewk_back_forward_list_item_original_url_get(item));
  ASSERT_STREQ(expectedTitle1.c_str(), ewk_back_forward_list_item_title_get(item));

  // go back
  ASSERT_EQ(EINA_TRUE, ewk_view_back(GetEwkWebView()));
  ASSERT_EQ(Success, EventLoopStart());

  // get item at 1 and check URL, original URL and title
  item = ewk_back_forward_list_item_at_index_get(list, 1);

  ASSERT_STREQ(GetResourceUrl(TEST_URL3).c_str(), ewk_back_forward_list_item_url_get(item));
  ASSERT_STREQ(GetResourceUrl(TEST_URL3).c_str(), ewk_back_forward_list_item_original_url_get(item));
  ASSERT_STREQ(expectedTitle3.c_str(), ewk_back_forward_list_item_title_get(item));

  // get item at 0 and check URL, original URL and title
  item = ewk_back_forward_list_item_at_index_get(list, 0);

  ASSERT_STREQ(GetResourceUrl(TEST_URL2).c_str(), ewk_back_forward_list_item_url_get(item));
  ASSERT_STREQ(GetResourceUrl(TEST_URL2).c_str(), ewk_back_forward_list_item_original_url_get(item));
  ASSERT_STREQ(expectedTitle2.c_str(), ewk_back_forward_list_item_title_get(item));

  // get item at -1 and check URL, original URL and title
  item = ewk_back_forward_list_item_at_index_get(list, -1);

  ASSERT_STREQ(GetResourceUrl(TEST_URL1).c_str(), ewk_back_forward_list_item_url_get(item));
  ASSERT_STREQ(GetResourceUrl(TEST_URL1).c_str(), ewk_back_forward_list_item_original_url_get(item));
  ASSERT_STREQ(expectedTitle1.c_str(), ewk_back_forward_list_item_title_get(item));

  // go back
  ASSERT_EQ(EINA_TRUE, ewk_view_back(GetEwkWebView()));
  ASSERT_EQ(Success, EventLoopStart());

  // get item at 2 and check URL, original URL and title
  item = ewk_back_forward_list_item_at_index_get(list, 2);

  ASSERT_STREQ(GetResourceUrl(TEST_URL3).c_str(), ewk_back_forward_list_item_url_get(item));
  ASSERT_STREQ(GetResourceUrl(TEST_URL3).c_str(), ewk_back_forward_list_item_original_url_get(item));
  ASSERT_STREQ(expectedTitle3.c_str(), ewk_back_forward_list_item_title_get(item));

  // get item at 1 and check URL, original URL and title
  item = ewk_back_forward_list_item_at_index_get(list, 1);

  ASSERT_STREQ(GetResourceUrl(TEST_URL2).c_str(), ewk_back_forward_list_item_url_get(item));
  ASSERT_STREQ(GetResourceUrl(TEST_URL2).c_str(), ewk_back_forward_list_item_original_url_get(item));
  ASSERT_STREQ(expectedTitle2.c_str(), ewk_back_forward_list_item_title_get(item));

  // get item at 0 and check URL, original URL and title
  item = ewk_back_forward_list_item_at_index_get(list, 0);

  ASSERT_STREQ(GetResourceUrl(TEST_URL1).c_str(), ewk_back_forward_list_item_url_get(item));
  ASSERT_STREQ(GetResourceUrl(TEST_URL1).c_str(), ewk_back_forward_list_item_original_url_get(item));
  ASSERT_STREQ(expectedTitle1.c_str(), ewk_back_forward_list_item_title_get(item));

  // get item at 10 and it should be NULL
  ASSERT_EQ(NULL, ewk_back_forward_list_item_at_index_get(list, 10));

  // get item at -10 and it should be NULL
  ASSERT_EQ(NULL, ewk_back_forward_list_item_at_index_get(list, -10));
}

TEST_F(utc_blink_ewk_view_back_forward_list_item_at_index_get, EMPTY_TEST)
{
  Ewk_Back_Forward_List *list = ewk_view_back_forward_list_get(GetEwkWebView());
  ASSERT_TRUE(list);
  ASSERT_EQ(NULL, ewk_back_forward_list_item_at_index_get(list, 0));
  ASSERT_EQ(NULL, ewk_back_forward_list_item_at_index_get(list, 1));
  ASSERT_EQ(NULL, ewk_back_forward_list_item_at_index_get(list, -1));
}

TEST_F(utc_blink_ewk_view_back_forward_list_item_at_index_get, NULL_TEST)
{
  ASSERT_EQ(NULL, ewk_back_forward_list_item_at_index_get(NULL, 0));
}

