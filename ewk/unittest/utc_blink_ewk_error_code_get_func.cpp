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

class utc_blink_ewk_error_code_get : public utc_blink_ewk_base {
protected:
  /* Callback for load error */
  bool LoadError(Evas_Object* webview, Ewk_Error* error)
  {
    utc_message("LoadError");
    int err = ewk_error_code_get(error);

    if (err != 0) {
      EventLoopStop(utc_blink_ewk_base::Success);
      return true;
    }

    // returning false will cause default behaviour - exiting main loop with Failure
    return false;
  }

  /* Callback for load progress */
  void LoadProgress(Evas_Object* webview, double progress)
  {
    utc_message("LoadProgress");
    ewk_view_stop(webview);
  }

protected:
  static const char* const test_url;
};

const char* const utc_blink_ewk_error_code_get::test_url = "http://google.com";

/**
 * @brief Positive test case of ewk_error_code_get(). Page is loaded and stopped in between to generate loadError
 */
TEST_F(utc_blink_ewk_error_code_get, POS_TEST)
{
  Eina_Bool result = ewk_view_url_set(GetEwkWebView(), test_url);

  if (!result) {
    utc_fail();
  }

  utc_blink_ewk_base::MainLoopResult loop_result = EventLoopStart();
  utc_check_eq(loop_result, utc_blink_ewk_base::Success);
}

/**
* @brief Checking whether function works properly in case of NULL argument.
*/
TEST_F(utc_blink_ewk_error_code_get, NEG_TEST)
{
  utc_check_eq(ewk_error_code_get(NULL), 0);
}
