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

class utc_blink_ewk_error_type_get : public utc_blink_ewk_base {
 protected:
  utc_blink_ewk_error_type_get()
    : error_type(EWK_ERROR_TYPE_NONE)
  {
  }

  /* Callback for load error */
  bool LoadError(Evas_Object* webview, Ewk_Error* error)
  {
    utc_message("[load_error] :: ");
    error_type = ewk_error_type_get(error);
    EventLoopStop(utc_blink_ewk_base::Success);
    return true;
  }

  /* Callback for load progress */
  void LoadProgress(Evas_Object* webview, double progress)
  {
    ewk_view_stop(webview);
  }

protected:
  Ewk_Error_Type error_type;
  static const char* const test_url;
};

const char* const utc_blink_ewk_error_type_get::test_url = "http://google.com";

/**
 * @brief Positive test case of ewk_error_type_get(). Page is loaded and stopped in between to generate loadError
 */
TEST_F(utc_blink_ewk_error_type_get, POS_TEST)
{
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), test_url));
  ASSERT_EQ(utc_blink_ewk_base::Success, EventLoopStart());
  ASSERT_EQ(EWK_ERROR_TYPE_NETWORK, error_type); // right now this is the only error type supported
}

/**
* @brief Checking whether function works properly in case of NULL argument.
*/
TEST_F(utc_blink_ewk_error_type_get, NEG_TEST)
{
  ASSERT_EQ(EWK_ERROR_TYPE_NONE, ewk_error_type_get(NULL));
}
