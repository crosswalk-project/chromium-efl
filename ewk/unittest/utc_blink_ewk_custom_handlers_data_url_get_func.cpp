// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_custom_handlers_data_url_get: public utc_blink_ewk_base {
protected:
  void PostSetUp()
  {
    evas_object_smart_callback_add(GetEwkWebView(),"protocolhandler,registration,requested", (void(*)(void*, Evas_Object*, void*))custom_handler, this);
  }

  void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(),"protocolhandler,registration,requested", (void(*)(void*, Evas_Object*, void*))custom_handler);
  }

  void LoadFinished(Evas_Object* webview)
  {
    utc_message("[loadFinished] :: ");
    EventLoopStop(Failure);
  }

  static void custom_handler(utc_blink_ewk_custom_handlers_data_url_get* owner, Evas_Object* /*webview*/, Ewk_Custom_Handlers_Data* custom_handler_data)
  {
    utc_message("[custom handler] :: \n");
    ASSERT_TRUE(NULL != owner);
    ASSERT_TRUE(NULL != custom_handler_data);

    ASSERT_STREQ("http://codebits.glennjones.net/registerprotocol/register.html?%s", ewk_custom_handlers_data_url_get(custom_handler_data));
    owner->EventLoopStop(Success);
  }
};

/**
 * @brief Checking if base_url is returned properly.
*/
TEST_F(utc_blink_ewk_custom_handlers_data_url_get, POS_TEST)
{
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), "http://codebits.glennjones.net/registerprotocol/register.html"));
  ASSERT_EQ(Success, EventLoopStart());
}

/**
 * @brief Checking if NULL is returned when custom_handler_data is NULL.
*/
TEST_F(utc_blink_ewk_custom_handlers_data_url_get, NEG_TEST)
{
  ASSERT_EQ(NULL, ewk_custom_handlers_data_url_get(NULL));
}
