// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

#define URL  ("http://request.urih.com/")
#define ACCEPT_LANGUAGES_DEFAULT_VALUE ("en-us,en")

class utc_blink_ewk_context_preferred_languages_set : public utc_blink_ewk_base
{
protected:
  utc_blink_ewk_context_preferred_languages_set()
  {
  }

  void LoadFinished(Evas_Object* webview)
  {
      ewk_view_plain_text_get(webview, _get_plain_text_cb, this);
  }

  static void _get_plain_text_cb(Evas_Object* obj, const char* result, void* data)
  {
    utc_blink_ewk_context_preferred_languages_set* owner = static_cast<utc_blink_ewk_context_preferred_languages_set*>(data);

    ASSERT_TRUE(owner);

    if (result != NULL) {
      owner->plain_text_result.append(result);
    }

    owner->EventLoopStop(utc_blink_ewk_base::Success);
  }

protected:
  std::string plain_text_result;
};

/**
 * @brief Positive test case of utc_blink_ewk_context_preferred_languages_set()
 */
TEST_F(utc_blink_ewk_context_preferred_languages_set, POS_TEST1)
{
  utc_message("Loading url: %s", URL);

  Eina_List *list = NULL;
  list = eina_list_append(list, "TEST");
  list = eina_list_append(list, "ACC_EPT");
  list = eina_list_append(list, "LA-NG");

  ewk_context_preferred_languages_set(list);

  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), URL));
  ASSERT_EQ(Success, EventLoopStart());

  ASSERT_NE(plain_text_result.find("test,acc-ept,la-ng"), std::string::npos);
}

/**
 * @brief Positive test case of utc_blink_ewk_context_preferred_languages_set()
 *        NULL value should make the AcceptLangs header to be set to default value.
 */
TEST_F(utc_blink_ewk_context_preferred_languages_set, POS_TEST2)
{
  utc_message("Loading url: %s", URL);

  ewk_context_preferred_languages_set(NULL);

  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), URL));
  ASSERT_EQ(Success, EventLoopStart());

  ASSERT_NE(plain_text_result.find(ACCEPT_LANGUAGES_DEFAULT_VALUE), std::string::npos);
}

