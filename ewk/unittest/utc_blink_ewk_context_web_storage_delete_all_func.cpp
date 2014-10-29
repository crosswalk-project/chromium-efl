// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/* Define those macros _before_ you include the utc_blink_ewk.h header file. */

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_context_web_storage_delete_all : public utc_blink_ewk_base
{
protected:

  void PostSetUp()
  {
    evas_object_smart_callback_add(GetEwkWebView(), "title,changed", title_changed, this);
  }

  void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(), "title,changed", title_changed);
  }

  static void title_changed(void* data, Evas_Object* webview, void* event_info)
  {
    ASSERT_TRUE(data != NULL);

    utc_blink_ewk_context_web_storage_delete_all* owner = static_cast<utc_blink_ewk_context_web_storage_delete_all*>(data);

    owner->EventLoopStop(utc_blink_ewk_base::Success);
  }
};

/**
* @brief Tests if can delete all web storages
*/
TEST_F(utc_blink_ewk_context_web_storage_delete_all, POS_TEST)
{
  std::string storageItemValue;

  // Set web storage item StorageTest=STORAGE_TEST_VALUE
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), GetResourceUrl("ewk_context_web_storage/webStorageSet.html").c_str()));
  ASSERT_EQ(Success, EventLoopStart());

  // Check if the storage was properly set
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), GetResourceUrl("ewk_context_web_storage/webStorageGet.html").c_str()));
  ASSERT_EQ(Success, EventLoopStart());

  storageItemValue = ewk_view_title_get(GetEwkWebView());
  ASSERT_EQ("STORAGE_TEST_VALUE", storageItemValue);
  EventLoopWait(3.0);

  // Delete all web storage
  Ewk_Context* context = ewk_view_context_get(GetEwkWebView());
  ASSERT_EQ(EINA_TRUE, ewk_context_web_storage_delete_all(context));

  // Check if the storage was really deleted
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), GetResourceUrl("ewk_context_web_storage/webStorageGet.html").c_str()));
  ASSERT_EQ(Success, EventLoopStart());

  storageItemValue = ewk_view_title_get(GetEwkWebView());
  EXPECT_EQ("EMPTY", storageItemValue);
}

/**
* @brief Tests if returns false when context is null.
*/
TEST_F(utc_blink_ewk_context_web_storage_delete_all, NEG_TEST)
{
  EXPECT_FALSE(ewk_context_web_storage_delete_all(0));
}
