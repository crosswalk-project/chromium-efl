// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"
#include <regex.h>

class utc_blink_ewk_view_get_cookies_for_url : public utc_blink_ewk_base
{
protected:
  void LoadFinished(Evas_Object* webview) {
    EventLoopStop(Success);
  }
};

/**
 * @brief Checking cookie after load page witch cookies
 */
TEST_F(utc_blink_ewk_view_get_cookies_for_url, POS_TEST)
{
  // http://wp.pl set cookie for each new request with out prompt user
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), "http://wp.pl/"));
  ASSERT_EQ(Success, EventLoopStart());
  const char* const url = ewk_view_url_get(GetEwkWebView());
  ASSERT_TRUE(url);
  utc_message("url=%s", url);
  char* cookies = ewk_view_get_cookies_for_url(GetEwkWebView(), url);
  ASSERT_TRUE(cookies);
  utc_message("cookies=%s", cookies);
  regex_t scope;
  ASSERT_TRUE(0 == regcomp(&scope, "^.*=*.(;.*=*.)*$",1 ));
  ASSERT_TRUE(0 == regexec(&scope, cookies, 0, 0, 0));
  regfree(&scope);
  free(cookies);
}

/**
 * @brief Checking cookie after load page without cookies
 */
TEST_F(utc_blink_ewk_view_get_cookies_for_url, NEG_TEST1)
{
  // Normaly resource have not cookie.
  // See http://wolfslittlestore.be/2012/08/cookies-on-local-files-in-chrome/
  // or http://stackoverflow.com/questions/12992494/how-to-read-write-cookies-for-local-file-html-document
  ASSERT_TRUE(ewk_view_url_set(GetEwkWebView(), GetResourceUrl("common/sample.html").c_str()));
  ASSERT_EQ(Success, EventLoopStart());
  const char* const url = ewk_view_url_get(GetEwkWebView());
  ASSERT_TRUE(url);
  utc_message("url=%s", url);
  ASSERT_TRUE(NULL == ewk_view_get_cookies_for_url(GetEwkWebView(), url));
}

/**
 * @brief Checking function works properly in case of invalid parameters.
 */

TEST_F(utc_blink_ewk_view_get_cookies_for_url, NEG_TEST2)
{
  ASSERT_TRUE(NULL == ewk_view_get_cookies_for_url(GetEwkWebView(),NULL));
  ASSERT_TRUE(NULL == ewk_view_get_cookies_for_url(NULL,"http://samsung.com/"));
}
