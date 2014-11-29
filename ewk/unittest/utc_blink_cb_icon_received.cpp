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

class utc_blink_cb_icon_received : public utc_blink_ewk_base
{
protected:
  void PostSetUp()
  {
    evas_object_smart_callback_add(GetEwkWebView(), "icon,received", cb_icon_received, this);
  }

  void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(), "icon,received", cb_icon_received);
  }

  static void cb_icon_received(void *data, Evas_Object *, void *)
  {
    ASSERT_TRUE(data != NULL);
    utc_blink_cb_icon_received *owner = static_cast<utc_blink_cb_icon_received *>(data);
    owner->EventLoopStop(Success);
  }
};

TEST_F(utc_blink_cb_icon_received, PAGE_WITH_FAVICON)
{
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), GetResourceUrl("common/favicon.html").c_str()));
  ASSERT_EQ(Success, EventLoopStart());
}

TEST_F(utc_blink_cb_icon_received, PAGE_WITHOUT_FAVICON)
{
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), GetResourceUrl("common/sample.html").c_str()));
  ASSERT_EQ(Failure, EventLoopStart(5));
}
