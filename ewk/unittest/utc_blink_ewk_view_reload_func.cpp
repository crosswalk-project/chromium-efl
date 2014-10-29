// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_view_reload : public utc_blink_ewk_base
{
protected:
  void LoadFinished(Evas_Object* webview)
  {
    EventLoopStop(Success);
  }

protected:
  static const char*const sample;
};

const char*const utc_blink_ewk_view_reload::sample="common/sample.html";

/**
 * @brief Tests if returns TRUE when not initiated with correct context with a valid page load.
 */
TEST_F(utc_blink_ewk_view_reload, POS_TEST)
{
  Eina_Bool loadResult = EINA_FALSE;
  Eina_Bool result= EINA_FALSE;
  if(!ewk_view_url_set(GetEwkWebView(),GetResourceUrl(sample).c_str()))
    utc_fail();
  if(Success!=EventLoopStart())
    utc_fail();

  loadResult= ewk_view_reload(GetEwkWebView());

  if (loadResult == EINA_TRUE)
    result = EINA_TRUE;

  evas_object_show(GetEwkWebView());
  evas_object_show(GetEwkWindow());
  utc_check_eq(result, EINA_TRUE);
}

/**
 * @brief Tests if returns FALSE when not initiated with NULL context.
 */
TEST_F(utc_blink_ewk_view_reload, NEG_TEST)
{
  Eina_Bool loadResult = EINA_FALSE;
  Eina_Bool result= EINA_FALSE;
  loadResult= ewk_view_reload(NULL);
  if (loadResult == EINA_TRUE)
    result = EINA_TRUE;
  evas_object_show(GetEwkWebView());
  evas_object_show(GetEwkWindow());
  utc_check_ne(result, EINA_TRUE);
}
