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
