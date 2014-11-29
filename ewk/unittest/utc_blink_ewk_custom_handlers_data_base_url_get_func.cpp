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

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_custom_handlers_data_base_url_get: public utc_blink_ewk_base {
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

  static void custom_handler(utc_blink_ewk_custom_handlers_data_base_url_get* owner, Evas_Object* /*webview*/, Ewk_Custom_Handlers_Data* custom_handler_data)
  {
    utc_message("[custom handler] :: \n");
    ASSERT_TRUE(NULL != owner);
    ASSERT_TRUE(NULL != custom_handler_data);

    ASSERT_STREQ("codebits.glennjones.net", ewk_custom_handlers_data_base_url_get(custom_handler_data));
    owner->EventLoopStop(Success);
  }
};

/**
 * @brief Checking if base_url is returned properly.
*/
TEST_F(utc_blink_ewk_custom_handlers_data_base_url_get, POS_TEST)
{
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), "http://codebits.glennjones.net/registerprotocol/register.html"));
  ASSERT_EQ(Success, EventLoopStart());
}

/**
 * @brief Checking if NULL is returned when custom_handler_data is NULL.
*/
TEST_F(utc_blink_ewk_custom_handlers_data_base_url_get, NEG_TEST)
{
  ASSERT_EQ(NULL, ewk_custom_handlers_data_base_url_get(NULL));
}
