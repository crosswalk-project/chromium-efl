// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_view_script_execute : public utc_blink_ewk_base
{
 protected:

  /* Callback for load finished */
  void LoadFinished(Evas_Object* webview)
  {
    EventLoopStop(Success);
  }

  /* Callback for script execution */
  static void scriptExecutionFinished(Evas_Object* webview, const char* result_value, void* data)
  {
    utc_message("[scriptExecutionFinished] ::");
    if(data)
      static_cast<utc_blink_ewk_view_script_execute*>(data)->EventLoopStop(Success);

    if(strcmp(result_value, "test content"))
      utc_fail();
  }

protected:
  static const char*const sample;
  static const char*const javaScript;
};

const char*const utc_blink_ewk_view_script_execute::sample="common/sample.html";
const char*const utc_blink_ewk_view_script_execute::javaScript="document.getElementById('getParaContent').innerHTML";

/**
* @brief Positive test case of ewk_view_script_execute(). Script is executed and its corresponding callback should get called.
*/
TEST_F(utc_blink_ewk_view_script_execute, POS_TEST)
{
  if(!ewk_view_url_set(GetEwkWebView(),GetResourceUrl(sample).c_str()))
     utc_fail();
  if(Success!=EventLoopStart())
    utc_fail();

  Eina_Bool result = ewk_view_script_execute(GetEwkWebView(), javaScript, scriptExecutionFinished, this);
  if(Success!=EventLoopStart())
    utc_fail();

  utc_check_eq(result, EINA_TRUE);
}

/**
* @brief Checking whether function works properly in case of NULL of a webview.
*/
TEST_F(utc_blink_ewk_view_script_execute, NEG_TEST)
{
  Eina_Bool result = ewk_view_script_execute(NULL, javaScript, scriptExecutionFinished, this);
  utc_check_eq(result, EINA_FALSE);
}
