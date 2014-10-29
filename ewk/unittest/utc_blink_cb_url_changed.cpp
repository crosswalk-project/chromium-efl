// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
