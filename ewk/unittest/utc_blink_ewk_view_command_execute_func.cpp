// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_view_command_execute : public utc_blink_ewk_base
{
protected:
  /* Callback for load finished */
  void LoadFinished(Evas_Object* webview)
  {
    EventLoopStop( Success );
  }
};

/**
* @brief Positive test case of ewk_view_command_execute(). Page is loaded and command "SelectAll" is executed.
*/
TEST_F(utc_blink_ewk_view_command_execute, POS_TEST)
{
  if(!ewk_view_url_set(GetEwkWebView(), GetResourceUrl("common/sample.html").c_str()))
    utc_fail();
  if(Success!=EventLoopStart())
    utc_fail();

  Eina_Bool result = ewk_view_command_execute(GetEwkWebView(), "SelectAll", NULL);
  utc_check_eq(result, EINA_TRUE);
}

/**
* @brief Checking whether function works properly in case of NULL of a webview.
*/
TEST_F(utc_blink_ewk_view_command_execute, NEG_TEST1)
{
  Eina_Bool result = ewk_view_command_execute(NULL, "SelectAll", NULL);
  utc_check_eq(result, EINA_FALSE);
}

/**
* @brief Checking whether function works properly in case of NULL of command.
*/
TEST_F(utc_blink_ewk_view_command_execute, NEG_TEST2)
{
  Eina_Bool result = ewk_view_command_execute(GetEwkWebView(), NULL, NULL);
  utc_check_eq(result, EINA_FALSE);
}
