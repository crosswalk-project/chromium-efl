// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

#ifndef NULL
#define NULL    0x0
#endif

class utc_blink_ewk_view_add : public utc_blink_ewk_base
{
};

/**
 * @brief Tests if returns valid webview when called with correct Evas object.
 */
TEST_F(utc_blink_ewk_view_add, POS_TEST)
{
  Eina_Bool result = EINA_FALSE;
  Evas_Object* window = elm_win_add(NULL, "TC Launcher", ELM_WIN_BASIC);
  elm_win_title_set(window, "TC Launcher");
  Evas *evas = evas_object_evas_get(window);

  /* Initialization of webview */
  Evas_Object* webview = ewk_view_add(evas);

  if( webview)
    result = EINA_TRUE;

  evas_object_show(webview);
  evas_object_show(window);

  evas_object_del(webview);
  evas_object_del(window);
  utc_check_eq(result, EINA_TRUE);
}

/**
 * @brief Tests if returns NULL when not called with NULL Evas object.
 */
TEST_F(utc_blink_ewk_view_add, NEG_TEST)
{
  Evas_Object* webview = ewk_view_add(NULL);
  if( webview)
    utc_fail();
}
