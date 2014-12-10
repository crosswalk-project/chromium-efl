// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_back_forward_list_n_back_items_copy : public utc_blink_ewk_base
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

const char* const utc_blink_ewk_back_forward_list_n_back_items_copy::TEST_URL1 = "ewk_history/page1.html";
const char* const utc_blink_ewk_back_forward_list_n_back_items_copy::TEST_URL2 = "ewk_history/page2.html";
const char* const utc_blink_ewk_back_forward_list_n_back_items_copy::TEST_URL3 = "ewk_history/page3.html";

TEST_F(utc_blink_ewk_back_forward_list_n_back_items_copy, LESS_ITEMS_TEST)
{
  // load 3 pages to get some interesting history
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), GetResourceUrl(TEST_URL1).c_str()));
  ASSERT_EQ(Success, EventLoopStart());
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), GetResourceUrl(TEST_URL2).c_str()));
  ASSERT_EQ(Success, EventLoopStart());
  std::string expectedTitle2(ewk_view_title_get(GetEwkWebView()));
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), GetResourceUrl(TEST_URL3).c_str()));
  ASSERT_EQ(Success, EventLoopStart());

  // get back-forward list
  Ewk_Back_Forward_List *list = ewk_view_back_forward_list_get(GetEwkWebView());
  ASSERT_TRUE(list);

  Eina_List *back_list = ewk_back_forward_list_n_back_items_copy(list, 1);
  ASSERT_TRUE(back_list);
  ASSERT_EQ(1, eina_list_count(back_list));

  // get the only item and check URL, original URL and title
  Ewk_Back_Forward_List_Item *item = static_cast<Ewk_Back_Forward_List_Item *>(back_list->data);

  ASSERT_STREQ(GetResourceUrl(TEST_URL2).c_str(), ewk_back_forward_list_item_url_get(item));
  ASSERT_STREQ(GetResourceUrl(TEST_URL2).c_str(), ewk_back_forward_list_item_original_url_get(item));
  ASSERT_STREQ(expectedTitle2.c_str(), ewk_back_forward_list_item_title_get(item));

  eina_list_free(back_list);
}

TEST_F(utc_blink_ewk_back_forward_list_n_back_items_copy, EQUAL_ITEMS_TEST)
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

  // get back-forward list
  Ewk_Back_Forward_List *list = ewk_view_back_forward_list_get(GetEwkWebView());
  ASSERT_TRUE(list);

  Eina_List *back_list = ewk_back_forward_list_n_back_items_copy(list, 2);
  ASSERT_TRUE(back_list);
  ASSERT_EQ(2, eina_list_count(back_list));

  // get first item and check URL, original URL and title
  Ewk_Back_Forward_List_Item *item = static_cast<Ewk_Back_Forward_List_Item *>(back_list->data);

  ASSERT_STREQ(GetResourceUrl(TEST_URL1).c_str(), ewk_back_forward_list_item_url_get(item));
  ASSERT_STREQ(GetResourceUrl(TEST_URL1).c_str(), ewk_back_forward_list_item_original_url_get(item));
  ASSERT_STREQ(expectedTitle1.c_str(), ewk_back_forward_list_item_title_get(item));

  // get second item and check URL, original URL and title
  item = static_cast<Ewk_Back_Forward_List_Item *>(back_list->next->data);

  ASSERT_STREQ(GetResourceUrl(TEST_URL2).c_str(), ewk_back_forward_list_item_url_get(item));
  ASSERT_STREQ(GetResourceUrl(TEST_URL2).c_str(), ewk_back_forward_list_item_original_url_get(item));
  ASSERT_STREQ(expectedTitle2.c_str(), ewk_back_forward_list_item_title_get(item));

  eina_list_free(back_list);

  back_list = ewk_back_forward_list_n_back_items_copy(list, -1);
  ASSERT_TRUE(back_list);
  ASSERT_EQ(2, eina_list_count(back_list));

  // get first item and check URL, original URL and title
  item = static_cast<Ewk_Back_Forward_List_Item *>(back_list->data);

  ASSERT_STREQ(GetResourceUrl(TEST_URL1).c_str(), ewk_back_forward_list_item_url_get(item));
  ASSERT_STREQ(GetResourceUrl(TEST_URL1).c_str(), ewk_back_forward_list_item_original_url_get(item));
  ASSERT_STREQ(expectedTitle1.c_str(), ewk_back_forward_list_item_title_get(item));

  // get second item and check URL, original URL and title
  item = static_cast<Ewk_Back_Forward_List_Item *>(back_list->next->data);

  ASSERT_STREQ(GetResourceUrl(TEST_URL2).c_str(), ewk_back_forward_list_item_url_get(item));
  ASSERT_STREQ(GetResourceUrl(TEST_URL2).c_str(), ewk_back_forward_list_item_original_url_get(item));
  ASSERT_STREQ(expectedTitle2.c_str(), ewk_back_forward_list_item_title_get(item));

  eina_list_free(back_list);
}

TEST_F(utc_blink_ewk_back_forward_list_n_back_items_copy, MORE_ITEMS_TEST)
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

  // get back-forward list
  Ewk_Back_Forward_List *list = ewk_view_back_forward_list_get(GetEwkWebView());
  ASSERT_TRUE(list);

  Eina_List *back_list = ewk_back_forward_list_n_back_items_copy(list, 10);
  ASSERT_TRUE(back_list);
  ASSERT_EQ(2, eina_list_count(back_list));

  // get first item and check URL, original URL and title
  Ewk_Back_Forward_List_Item *item = static_cast<Ewk_Back_Forward_List_Item *>(back_list->data);

  ASSERT_STREQ(GetResourceUrl(TEST_URL1).c_str(), ewk_back_forward_list_item_url_get(item));
  ASSERT_STREQ(GetResourceUrl(TEST_URL1).c_str(), ewk_back_forward_list_item_original_url_get(item));
  ASSERT_STREQ(expectedTitle1.c_str(), ewk_back_forward_list_item_title_get(item));

  // get second item and check URL, original URL and title
  item = static_cast<Ewk_Back_Forward_List_Item *>(back_list->next->data);

  ASSERT_STREQ(GetResourceUrl(TEST_URL2).c_str(), ewk_back_forward_list_item_url_get(item));
  ASSERT_STREQ(GetResourceUrl(TEST_URL2).c_str(), ewk_back_forward_list_item_original_url_get(item));
  ASSERT_STREQ(expectedTitle2.c_str(), ewk_back_forward_list_item_title_get(item));

  eina_list_free(back_list);
}

TEST_F(utc_blink_ewk_back_forward_list_n_back_items_copy, EMPTY_TEST)
{
  Ewk_Back_Forward_List *list = ewk_view_back_forward_list_get(GetEwkWebView());
  ASSERT_TRUE(list);
  Eina_List *back_list = ewk_back_forward_list_n_back_items_copy(list, -1);
  ASSERT_EQ(0, eina_list_count(back_list));
}

TEST_F(utc_blink_ewk_back_forward_list_n_back_items_copy, NULL_TEST)
{
  ASSERT_FALSE(ewk_back_forward_list_n_back_items_copy(NULL, -1));
}
