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

class utc_blink_ewk_view_scroll_by : public utc_blink_ewk_base
{
protected:
  static void scroll_cb(utc_blink_ewk_view_scroll_by* owner, Evas_Object* eObject, void*)
  {
    utc_message("[%s]", __PRETTY_FUNCTION__);
    ASSERT_TRUE(owner);
    owner->EventLoopStop(Success);
  }

   /* Startup function */
  void PostSetUp()
  {
    evas_object_smart_callback_add(GetEwkWebView(), "scroll,right", ToSmartCallback(scroll_cb), this);
    evas_object_smart_callback_add(GetEwkWebView(), "scroll,left", ToSmartCallback(scroll_cb), this);
    evas_object_smart_callback_add(GetEwkWebView(), "scroll,up", ToSmartCallback(scroll_cb), this);
    evas_object_smart_callback_add(GetEwkWebView(), "scroll,down", ToSmartCallback(scroll_cb), this);
  }

  /* Cleanup function */
  void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(), "scroll,right", ToSmartCallback(scroll_cb));
    evas_object_smart_callback_del(GetEwkWebView(), "scroll,left", ToSmartCallback(scroll_cb));
    evas_object_smart_callback_del(GetEwkWebView(), "scroll,up", ToSmartCallback(scroll_cb));
    evas_object_smart_callback_del(GetEwkWebView(), "scroll,down", ToSmartCallback(scroll_cb));
  }

  void LoadFinished(Evas_Object* webview)
  {
    EventLoopStop(Success);
  }

protected:
  static const char* const resource;
};

const char* const utc_blink_ewk_view_scroll_by::resource="/ewk_view/index_big_red_square.html";

/**
 * @brief Positive test case of ewk_view_scroll_by(),tseting scroll postion against after set, and scroll
 */

TEST_F(utc_blink_ewk_view_scroll_by, scroll_by_x)
{
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(),GetResourceUrl(resource).c_str()));
  ASSERT_EQ(Success, EventLoopStart());

  int offset = 10;
  int currentX, currentY;
  ASSERT_EQ(EINA_TRUE, ewk_view_scroll_pos_get(GetEwkWebView(), &currentX, &currentY));
  ASSERT_EQ(0, currentX);
  ASSERT_EQ(0, currentY);

  ewk_view_scroll_by(GetEwkWebView(), offset, 0);
  ASSERT_EQ(Success, EventLoopStart()); // scroll,down smart callback should stop event loop
  int newX, newY;
  ASSERT_EQ(EINA_TRUE, ewk_view_scroll_pos_get(GetEwkWebView(), &newX, &newY));
  ASSERT_EQ(currentY, newY); // shouldn't change
  ASSERT_EQ(currentX + offset, newX);
  currentX = newX;

  ewk_view_scroll_by(GetEwkWebView(), 0, offset);
  ASSERT_EQ(Success, EventLoopStart());
  ASSERT_EQ(EINA_TRUE, ewk_view_scroll_pos_get(GetEwkWebView(), &newX, &newY));
  ASSERT_EQ(currentX, newX);
  ASSERT_EQ(currentY + offset, newY);
  currentY = newY;

  ewk_view_scroll_by(GetEwkWebView(), offset, offset);
  ASSERT_EQ(Success, EventLoopStart());
  ASSERT_EQ(EINA_TRUE, ewk_view_scroll_pos_get(GetEwkWebView(), &newX, &newY));
  ASSERT_EQ(currentX + offset, newX);
  ASSERT_EQ(currentY + offset, newY);
}

