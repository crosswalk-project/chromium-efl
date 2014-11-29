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

class utc_blink_ewk_view_scroll_size_get : public utc_blink_ewk_base
{
 protected:
  void PostSetUp()
  {
    evas_object_smart_callback_add(GetEwkWebView(), "frame,rendered", frameRendered, this);
  }

  void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(), "frame,rendered", frameRendered);
  }

  static void frameRendered(void* data, Evas_Object* eObject, void* dataFinished)
  {
    utc_message("[frameRendered] :: \n");
    if(data)
      static_cast<utc_blink_ewk_view_scroll_size_get*>(data)->EventLoopStop(Success);
  }

protected:
  static const char*const resource;
};

const char*const utc_blink_ewk_view_scroll_size_get::resource="/ewk_view/index_big_red_square.html";

/**
 * @brief Positive test case of ewk_view_scroll_size_get(), API should return with true when passing correct arguments.
 */
TEST_F(utc_blink_ewk_view_scroll_size_get, POS_TEST)
{
  if(!ewk_view_url_set(GetEwkWebView(),GetResourceUrl(resource).c_str()))
    utc_fail();
  if(Success!=EventLoopStart())
    utc_fail();

  int xPossibleScrollArea, yPossibleScrollArea;
  utc_check_eq(ewk_view_scroll_size_get(GetEwkWebView(), &xPossibleScrollArea, &yPossibleScrollArea), EINA_TRUE);
}

/**
  * @brief Negative test case of ewk_view_scroll_size_get(), testing for null.
  */
TEST_F(utc_blink_ewk_view_scroll_size_get, NEG_TEST)
{
  int xPossibleScrollArea, yPossibleScrollArea;
  utc_check_eq(ewk_view_scroll_size_get(NULL, &xPossibleScrollArea,&yPossibleScrollArea), EINA_FALSE);
}
