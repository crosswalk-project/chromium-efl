// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_custom_handlers_data_result_set : public utc_blink_ewk_base {
protected:
  void PostSetUp()
  {
    evas_object_smart_callback_add(GetEwkWebView(),"protocolhandler,registration,requested", custom_handler, this);
  }

  void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(),"protocolhandler,registration,requested", custom_handler);
  }

  void LoadFinished(Evas_Object* webview)
  {
    utc_message("[loadFinished] :: ");
    EventLoopStop(Failure);
  }

  static void custom_handler(void* data, Evas_Object* /*webview*/, void* event_info)
  {
    utc_message("[custom handler] :: \n");

    ASSERT_TRUE(data != NULL);
    ASSERT_TRUE(event_info != NULL);

    utc_blink_ewk_custom_handlers_data_result_set* owner = static_cast<utc_blink_ewk_custom_handlers_data_result_set*>(data);
    Ewk_Custom_Handlers_Data* custom_handler_data = static_cast<Ewk_Custom_Handlers_Data*>(event_info);

    ewk_custom_handlers_data_result_set(custom_handler_data, EWK_CUSTOM_HANDLERS_DECLINED);
    owner->EventLoopStop(Success);
  }

protected:
  static const char* const URL;
};

const char* const utc_blink_ewk_custom_handlers_data_result_set::URL = "http://codebits.glennjones.net/registerprotocol/register.html";

/**
 * @brief Checking whether sending cancellation notification for authentication challenge works properly.
*/
TEST_F(utc_blink_ewk_custom_handlers_data_result_set, POS_TEST)
{
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), URL));
  ASSERT_EQ(Success, EventLoopStart());
}

/**
 * @brief Checking whether function works properly in case of NULL of a custom_handler_data.
*/
TEST_F(utc_blink_ewk_custom_handlers_data_result_set, NEG_TEST)
{
  ewk_custom_handlers_data_result_set(NULL, EWK_CUSTOM_HANDLERS_DECLINED);
}
