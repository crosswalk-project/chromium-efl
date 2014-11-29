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

class utc_blink_cb_fullscreen_enterfullscreen : public utc_blink_ewk_base
{
 protected:

  void PostSetUp()
  {
    Ewk_Settings* settings = ewk_view_settings_get(GetEwkWebView());
    ASSERT_TRUE(settings);
    ewk_settings_fullscreen_enabled_set(settings, EINA_TRUE);
    evas_object_smart_callback_add(GetEwkWebView(), "fullscreen,enterfullscreen",
                                   ToSmartCallback(fullscreen_enterfullscreen),
                                   this);
  }

  void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(), "fullscreen,enterfullscreen",
                                   ToSmartCallback(fullscreen_enterfullscreen));
  }

  void LoadFinished(Evas_Object* /*webview*/) {
    EventLoopStop(Success);
  }

  static void fullscreen_enterfullscreen(utc_blink_cb_fullscreen_enterfullscreen* owner,
                                        Evas_Object*, void*)
  {
    utc_message("\e[0;32m[ %s ]\e[0m", __PRETTY_FUNCTION__);
    ASSERT_TRUE(owner);
    owner->EventLoopStop(Success);
  }

};

TEST_F(utc_blink_cb_fullscreen_enterfullscreen, Canvas)
{
  ASSERT_TRUE(ewk_view_url_set(GetEwkWebView(),
                               GetResourceUrl("ewk_view/canvas.html").c_str()));
  // Wait for load finish
  ASSERT_EQ(Success, EventLoopStart());
  ewk_view_script_execute(GetEwkWebView(),
                          "document.getElementById('c').webkitRequestFullscreen()", NULL, this);
  // Wait for fullscreen_enterfullscreen
  EXPECT_EQ(Success, EventLoopStart());
}

TEST_F(utc_blink_cb_fullscreen_enterfullscreen, Video)
{
  ASSERT_TRUE(ewk_view_url_set(GetEwkWebView(),
                               GetResourceUrl("ewk_view/fullscreen.html").c_str()));
  // Wait for load finish
  ASSERT_EQ(Success, EventLoopStart());
  ewk_view_script_execute(GetEwkWebView(),
                          "document.getElementById('video').webkitRequestFullscreen()", NULL, this);
  // Wait for fullscreen_enterfullscreen
  EXPECT_EQ(Success, EventLoopStart());
}


