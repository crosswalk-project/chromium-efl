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

#define URL  ("http://google.com")

class utc_blink_cb_load_started : public utc_blink_ewk_base
{
protected:

  void LoadStarted(Evas_Object* webview)
  {
    EventLoopStop(utc_blink_ewk_base::Success);
  }

  void LoadFinished(Evas_Object* webview)
  {
    EventLoopStop(utc_blink_ewk_base::Failure);
  }
};

/**
 * @brief Tests "load,started" callback
 */
TEST_F(utc_blink_cb_load_started, callback)
{
    ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), URL));
    EXPECT_EQ(utc_blink_ewk_base::Success, EventLoopStart());
}
