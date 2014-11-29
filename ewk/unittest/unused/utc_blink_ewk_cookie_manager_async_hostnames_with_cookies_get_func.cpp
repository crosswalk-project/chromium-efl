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

/* Define those macros _before_ you include the utc_blink_ewk.h header file. */
#define TESTED_FUN_NAME ewk_cookie_manager_async_hostnames_with_cookies_get
#define POSITIVE_TEST_FUN_NUM 1
#define NEGATIVE_TEST_FUN_NUM 2

#include "utc_blink_ewk.h"

#ifndef NULL
#define NULL    0x0
#endif

extern struct Ewk_Test_View test_view;

class utc_blink_ewk_cookie_manager_async_hostnames_with_cookies_get : public ::testing::Test
{
 protected:
  utc_blink_ewk_cookie_manager_async_hostnames_with_cookies_get()
  {
  }

  ~utc_blink_ewk_cookie_manager_async_hostnames_with_cookies_get()
  {
  }

  static void getHostnamesWithCookiesCallback(Eina_List* hostnames, Ewk_Error* error, void* event_info)
  {
    if(error)
        utc_fail();

    Eina_List** ret = (Eina_List**)event_info;
    Eina_List* l;
    void* data;
    EINA_LIST_FOREACH(hostnames, l, data)
        *ret = eina_list_append(*ret, eina_stringshare_ref((char*)data));
    ecore_main_loop_quit();
  }

  static Eina_List* getHostnamesWithCookies(Ewk_Cookie_Manager* manager)
  {
    Eina_List* ret = 0;
    ewk_cookie_manager_async_hostnames_with_cookies_get(manager, getHostnamesWithCookiesCallback, &ret);
    ecore_main_loop_begin();
    return ret;
  }

  /* Callback for load finished */
  static void loadFinished(void *data, Evas_Object *webview, void *arg)
  {
    utc_message("[loadFinished] :: \n");
    utc_blink_main_loop_quit();
  }

  /* Callback for load error */
  static void loadError(void* data, Evas_Object* webview, void* event_info)
  {
    utc_message("[loadError] :: \n");
    utc_blink_main_loop_quit();

    utc_fail();
  }

  /* Startup function */
  void SetUp()
  {
    utc_blink_ewk_test_init();
    evas_object_smart_callback_add(test_view.webview, "load,finished", loadFinished, NULL);
    evas_object_smart_callback_add(test_view.webview, "load,error", loadError, NULL);
  }

  /* Cleanup function */
  void TearDown()
  {
    evas_object_smart_callback_del(test_view.webview, "load,finished", loadFinished);
    evas_object_smart_callback_del(test_view.webview, "load,error", loadError);
    utc_blink_ewk_test_end();
  }
};

/**
 * @brief Positive test case of ewk_cookie_manager_async_hostnames_with_cookies_get()
 */
TEST_F(utc_blink_ewk_cookie_manager_async_hostnames_with_cookies_get, POS_TEST)
{
  Ewk_Cookie_Manager* cookieManager = ewk_context_cookie_manager_get(ewk_view_context_get(test_view.webview));

  if (!cookieManager)
    utc_fail();

  ewk_view_url_set(test_view.webview, "http://www.google.com");
  // Wait until load finished.
  utc_blink_main_loop_begin();

  ewk_view_url_set(test_view.webview, "http://m.naver.com");
  // Wait until load finished.
  utc_blink_main_loop_begin();

  Eina_List* cookieNameList = getHostnamesWithCookies(cookieManager);
  if (eina_list_count(cookieNameList))
    utc_pass();
}

/**
 * @brief Negative test case of ewk_cookie_manager_async_hostnames_with_cookies_get()
 */
TEST_F(utc_blink_ewk_cookie_manager_async_hostnames_with_cookies_get, NEG_TEST1)
{
  Ewk_Cookie_Manager* cookieManager = ewk_context_cookie_manager_get(ewk_view_context_get(test_view.webview));

  if (!cookieManager)
    utc_fail();

  Eina_List* ret = 0;
  ewk_cookie_manager_async_hostnames_with_cookies_get(cookieManager, NULL, &ret);

  utc_pass();
}

/**
 * @brief Negative test case of ewk_cookie_manager_async_hostnames_with_cookies_get()
 */
TEST_F(utc_blink_ewk_cookie_manager_async_hostnames_with_cookies_get, NEG_TEST2)
{
  Eina_List* ret = 0;
  ewk_cookie_manager_async_hostnames_with_cookies_get(NULL, getHostnamesWithCookiesCallback, &ret);

  utc_pass();
}
