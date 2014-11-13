// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_back_forward_list_item_unref : public utc_blink_ewk_base
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

const char* const utc_blink_ewk_back_forward_list_item_unref::TEST_URL1 = "ewk_history/page1.html";
const char* const utc_blink_ewk_back_forward_list_item_unref::TEST_URL2 = "ewk_history/page2.html";
const char* const utc_blink_ewk_back_forward_list_item_unref::TEST_URL3 = "ewk_history/page3.html";

TEST_F(utc_blink_ewk_back_forward_list_item_unref, POS_TEST)
{
  // load 3 pages to get some interesting history
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), GetResourceUrl(TEST_URL1).c_str()));
  ASSERT_EQ(Success, EventLoopStart());
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), GetResourceUrl(TEST_URL2).c_str()));
  ASSERT_EQ(Success, EventLoopStart());

  // get back-forward list
  Ewk_Back_Forward_List *list = ewk_view_back_forward_list_get(GetEwkWebView());
  ASSERT_TRUE(list);

  // get current item and check URL, original URL and title
  Ewk_Back_Forward_List_Item *item = ewk_back_forward_list_current_item_get(list);
  ASSERT_STREQ(GetResourceUrl(TEST_URL2).c_str(), ewk_back_forward_list_item_url_get(item));

  // increase ref count
  item = ewk_back_forward_list_item_ref(item);
  ASSERT_TRUE(item);

  // go back
  ASSERT_EQ(EINA_TRUE, ewk_view_back(GetEwkWebView()));
  ASSERT_EQ(Success, EventLoopStart());

  // load page so the refed item should normally be deleted but as it is refed
  // it should be still ins the memory
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), GetResourceUrl(TEST_URL3).c_str()));
  ASSERT_EQ(Success, EventLoopStart());

  // lets check it url
  ASSERT_STREQ(GetResourceUrl(TEST_URL2).c_str(), ewk_back_forward_list_item_url_get(item));

  // and unref so it should be deleted
  ewk_back_forward_list_item_unref(item);
  // but there is no way to test it...
  SUCCEED();
}

TEST_F(utc_blink_ewk_back_forward_list_item_unref, NULL_TEST)
{
  ewk_back_forward_list_item_unref(NULL);
  SUCCEED();
}
