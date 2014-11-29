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

class utc_blink_ewk_view_text_find : public utc_blink_ewk_base {
protected:
  static const char* SAMPLE_HTML_FILE;
  static const char* SEARCH_STRING;
  static const int MAX_MATCH_COUNT;

protected:
  /* Callback for find string operation */
  static void findStringCallback(void* data, Evas_Object* webview, void* event_info)
  {
    utc_message("[findStringCallback] :: ");
    utc_blink_ewk_view_text_find *owner = static_cast<utc_blink_ewk_view_text_find*>(data);
    owner->EventLoopStop(utc_blink_ewk_base::Success); // will noop if EventLoopStop was alraedy called

    int count = *(unsigned int*)event_info;
    if (/*(strcmp(searchString, SEARCH_STRING)) ||*/ count == 0) {
        FAIL();
    }
  }

  /* Callback for load finished */
  void LoadFinished(Evas_Object* webview)
  {
    EventLoopStop(utc_blink_ewk_base::Success); // will noop if EventLoopStop was alraedy called
  }

  virtual void PostSetUp()
  {
    evas_object_smart_callback_add(GetEwkWebView(), "text,found", findStringCallback, this);
  }

  virtual void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(), "text,found", findStringCallback);
  }

};

const char* utc_blink_ewk_view_text_find::SAMPLE_HTML_FILE = "/common/sample.html";
const char* utc_blink_ewk_view_text_find::SEARCH_STRING = "test";
const int utc_blink_ewk_view_text_find::MAX_MATCH_COUNT = 2;

/**
* @brief Positive test case of ewk_view_string_find(). Search string and find options are set, and the result is returned in callback.
*/
TEST_F(utc_blink_ewk_view_text_find, POS_TEST)
{
  Eina_Bool resultBool = ewk_view_url_set(GetEwkWebView(), GetResourceUrl(SAMPLE_HTML_FILE).c_str());
  if (!resultBool) {
    FAIL();
  }

  utc_blink_ewk_base::MainLoopResult result = EventLoopStart();
  if (result != utc_blink_ewk_base::Success) {
    FAIL();
  }

  resultBool = ewk_view_text_find(GetEwkWebView(), SEARCH_STRING, EWK_FIND_OPTIONS_CASE_INSENSITIVE, MAX_MATCH_COUNT);
  EXPECT_EQ(resultBool, EINA_TRUE);
}

/**
* @brief Checking whether function works properly in case of NULL of a webview.
*/
TEST_F(utc_blink_ewk_view_text_find, NEG_TEST1)
{
  Eina_Bool result = ewk_view_text_find(NULL, SEARCH_STRING, EWK_FIND_OPTIONS_CASE_INSENSITIVE, MAX_MATCH_COUNT);
  EXPECT_EQ(result, EINA_FALSE);
}

/**
* @brief Checking whether function works properly in case of NULL of search string.
*/
TEST_F(utc_blink_ewk_view_text_find, NEG_TEST2)
{
  Eina_Bool result = ewk_view_text_find(GetEwkWebView(), NULL, EWK_FIND_OPTIONS_CASE_INSENSITIVE, MAX_MATCH_COUNT);
  EXPECT_EQ(result, EINA_FALSE);
}

