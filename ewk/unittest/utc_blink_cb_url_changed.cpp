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

class utc_blink_cb_url_changed : public utc_blink_ewk_base
{
protected:
  void PostSetUp()
  {
    evas_object_smart_callback_add(GetEwkWebView(), "url,changed", url_changed, this);
  }

  void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(), "url,changed", url_changed);
  }

  static void url_changed(void* data, Evas_Object* webview, void* arg)
  {
    ASSERT_TRUE(arg != NULL);
    ASSERT_TRUE(data != NULL);
    const char*const url = static_cast<char*>(arg);
    utc_blink_cb_url_changed*const owner = static_cast<utc_blink_cb_url_changed*>(data);
    utc_message("[url_changed]\t%s", url);
    if (owner->GetResourceUrl("common/sample.html") == url)
      owner->EventLoopStop(Success);
  }
};

/**
 * @brief Tests load page with refresh meta tag.
 */
TEST_F(utc_blink_cb_url_changed, Refresh)
{
  ASSERT_TRUE(ewk_view_url_set(GetEwkWebView(), GetResourceUrl("ewk_view/redirect.html").c_str()));
  EXPECT_EQ(Success, EventLoopStart()); // Wait for "url,changed" to .../sample.html
}

/**
 * @brief Tests load page without refresh meta tag.
 */
TEST_F(utc_blink_cb_url_changed, NoRefresh)
{
  ASSERT_TRUE(ewk_view_url_set( GetEwkWebView(), GetResourceUrl("common/sample_1.html").c_str()));
  EXPECT_EQ(Failure, EventLoopStart(5)); // Check for not redirect to .../sample.html
}
