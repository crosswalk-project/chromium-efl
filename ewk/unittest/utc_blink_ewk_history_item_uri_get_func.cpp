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

class utc_blink_ewk_history_item_uri_get : public utc_blink_ewk_base
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

const char* const utc_blink_ewk_history_item_uri_get::TEST_URL1 = "ewk_history/page1.html";
const char* const utc_blink_ewk_history_item_uri_get::TEST_URL2 = "ewk_history/page2.html";
const char* const utc_blink_ewk_history_item_uri_get::TEST_URL3 = "ewk_history/page3.html";

TEST_F(utc_blink_ewk_history_item_uri_get, EXISTING_HISTORY)
{
  // load 3 pages to get some interesting history
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), GetResourceUrl(TEST_URL1).c_str()));
  ASSERT_EQ(Success, EventLoopStart());
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), GetResourceUrl(TEST_URL2).c_str()));
  ASSERT_EQ(Success, EventLoopStart());
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), GetResourceUrl(TEST_URL3).c_str()));
  ASSERT_EQ(Success, EventLoopStart());

  // go back
  ASSERT_EQ(EINA_TRUE, ewk_view_back(GetEwkWebView()));

  Ewk_History *history = ewk_view_history_get(GetEwkWebView());
  ASSERT_TRUE(history);

  // get current item
  Ewk_History_Item *item = ewk_history_nth_item_get(history, 0);

  ASSERT_TRUE(item);
  ASSERT_STREQ(GetResourceUrl(TEST_URL2).c_str(), ewk_history_item_uri_get(item));

  // get back item
  item = ewk_history_nth_item_get(history, -1);

  ASSERT_TRUE(item);
  ASSERT_STREQ(GetResourceUrl(TEST_URL1).c_str(), ewk_history_item_uri_get(item));

  // get forward item
  item = ewk_history_nth_item_get(history, 1);

  ASSERT_TRUE(item);
  ASSERT_STREQ(GetResourceUrl(TEST_URL3).c_str(), ewk_history_item_uri_get(item));

  ewk_history_free(history);
}

TEST_F(utc_blink_ewk_history_item_uri_get, NULL_ITEM)
{
  ASSERT_FALSE(ewk_history_item_uri_get(NULL));
}

