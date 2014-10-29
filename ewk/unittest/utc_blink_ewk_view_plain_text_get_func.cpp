// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_view_plain_text_get : public utc_blink_ewk_base
{
 protected:
  /* Callback for load finished */
  void LoadFinished(Evas_Object* webview)
  {
    EventLoopStop(Success);
  }

  /* Ewk_View_Plain_Text_Get_Callback to get the text */
  static void getPlainTextFinished(Evas_Object* webview, const char* plain_text, void* data)
  {
    if(!data)
      utc_fail();
    utc_blink_ewk_view_plain_text_get*owner=static_cast<utc_blink_ewk_view_plain_text_get*>(data);
    utc_message("[getPlainTextFinished] ::");
    owner->EventLoopStop(Success);
    if(!plain_text)
      utc_fail();
  }

protected:
  static const char*const sample;
};

const char*const utc_blink_ewk_view_plain_text_get::sample="common/sample.html";

/**
* @brief Positive test case of ewk_view_plain_text_get(). Page is loaded, API is called and text is returned in corresponding callback.
*/
TEST_F(utc_blink_ewk_view_plain_text_get, POS_TEST)
{
  if(!ewk_view_url_set(GetEwkWebView(), GetResourceUrl(sample).c_str()))
    utc_fail();
  if(Success!=EventLoopStart())
    utc_fail();
  Eina_Bool result = ewk_view_plain_text_get(GetEwkWebView(), getPlainTextFinished, this);
  if(Success!=EventLoopStart())
    utc_fail();
  utc_check_eq(result, EINA_TRUE);
}

/**
* @brief Checking whether function works properly in case of NULL of a webview.
*/
TEST_F(utc_blink_ewk_view_plain_text_get, NEG_TEST1)
{
  Eina_Bool result = ewk_view_plain_text_get(NULL, getPlainTextFinished, NULL);
  utc_check_eq(result, EINA_FALSE);
}

/**
* @brief Checking whether function works properly in case of NULL of a callback.
*/
TEST_F(utc_blink_ewk_view_plain_text_get, NEG_TEST2)
{
  Eina_Bool result = ewk_view_plain_text_get(GetEwkWebView(), NULL, NULL);
  utc_check_eq(result, EINA_FALSE);
}
