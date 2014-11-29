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

class utc_blink_ewk_view_contents_size_get : public utc_blink_ewk_base
{
  /* Callback for load finished */
  void LoadFinished(Evas_Object* webview)
  {
    utc_message("[loadFinished] :: \n");
    EventLoopStop( Success );
  }
};

/**
 * @brief Positive test case of ewk_view_contents_size_get(). Page is loaded and API is called to get content size.
 */
TEST_F(utc_blink_ewk_view_contents_size_get, POS_TEST)
{
  if(!ewk_view_url_set(GetEwkWebView(), GetResourceUrl("common/sample.html").c_str()))
    utc_fail();
  if(Success!=EventLoopStart())
    utc_fail();

  Evas_Coord width = 0;
  Evas_Coord height = 0;

  Eina_Bool result = ewk_view_contents_size_get(GetEwkWebView(), &width, &height);

  if (width == 0 || height == 0)
    result = EINA_FALSE;

  utc_check_eq(result, EINA_TRUE);
}

/**
 * @brief Checking whether function works properly in case of NULL of a webview.
 */
TEST_F(utc_blink_ewk_view_contents_size_get, NEG_TEST)
{
  Evas_Coord width = 0;
  Evas_Coord height = 0;

  Eina_Bool result = ewk_view_contents_size_get(NULL, &width, &height);
  utc_check_eq(result, EINA_FALSE);
}
