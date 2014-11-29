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

class utc_blink_ewk_history_forward_list_length_get : public utc_blink_ewk_base
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

const char* const utc_blink_ewk_history_forward_list_length_get::TEST_URL1 = "ewk_history/page1.html";
const char* const utc_blink_ewk_history_forward_list_length_get::TEST_URL2 = "ewk_history/page2.html";
const char* const utc_blink_ewk_history_forward_list_length_get::TEST_URL3 = "ewk_history/page3.html";

TEST_F(utc_blink_ewk_history_forward_list_length_get, EMPTY_TEST)
{
  Ewk_History *history = ewk_view_history_get(GetEwkWebView());
  ASSERT_TRUE(history);
  ASSERT_EQ(0, ewk_history_forward_list_length_get(history));

  // free history
  ewk_history_free(history);
}

TEST_F(utc_blink_ewk_history_forward_list_length_get, NOT_EMPTY_TEST)
{
  // load first page
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), GetResourceUrl(TEST_URL1).c_str()));
  ASSERT_EQ(Success, EventLoopStart());

  // load second page
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), GetResourceUrl(TEST_URL2).c_str()));
  ASSERT_EQ(Success, EventLoopStart());

  // load third page
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), GetResourceUrl(TEST_URL3).c_str()));
  ASSERT_EQ(Success, EventLoopStart());

  // get history
  Ewk_History *history = ewk_view_history_get(GetEwkWebView());
  ASSERT_TRUE(history);

  ASSERT_EQ(0, ewk_history_forward_list_length_get(history));

  // free history
  ewk_history_free(history);

  // go back
  ASSERT_EQ(EINA_TRUE, ewk_view_back(GetEwkWebView()));
  ASSERT_EQ(Success, EventLoopStart());

  // get history
  history = ewk_view_history_get(GetEwkWebView());
  ASSERT_TRUE(history);

  ASSERT_EQ(1, ewk_history_forward_list_length_get(history));

  // free history
  ewk_history_free(history);

  // go back
  ASSERT_EQ(EINA_TRUE, ewk_view_back(GetEwkWebView()));
  ASSERT_EQ(Success, EventLoopStart());

  // get history
  history = ewk_view_history_get(GetEwkWebView());
  ASSERT_TRUE(history);

  ASSERT_EQ(2, ewk_history_forward_list_length_get(history));

  // free history
  ewk_history_free(history);
}

TEST_F(utc_blink_ewk_history_forward_list_length_get, NULL_HISTORY_TEST)
{
  ASSERT_EQ(0, ewk_history_forward_list_length_get(NULL));
}
