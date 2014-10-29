// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_cb_form_submit : public utc_blink_ewk_base
{
protected:
  void PostSetUp()
  {
    evas_object_smart_callback_add(GetEwkWebView(), "form,submit", (void (*)(void*, Evas_Object*, void*)) form_submit, this);
  }

  void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(), "form,submit", (void (*)(void*, Evas_Object*, void*)) form_submit);
  }

  static void form_submit(utc_blink_cb_form_submit* owner, Evas_Object* webview, const char* arg)
  {
    ASSERT_TRUE(NULL != owner);
    ASSERT_TRUE(NULL != arg);
    utc_message("[form_submit]\t%s",arg);
    EXPECT_STREQ(arg,"www.webkit.org/");
    owner->EventLoopStop(Success);
  }
};

/**
 * @brief Load page and submit form from javascript.
 */
TEST_F(utc_blink_cb_form_submit, AutoSubmit)
{
  ASSERT_TRUE(ewk_view_url_set(GetEwkWebView(), GetResourceUrl("ewk_form_data/index.html").c_str()));
  EXPECT_EQ(Success, EventLoopStart()); // Wait for "form,submit"
}

/**
 * @brief Load page and waiting for timeout (don't submit form).
 */
TEST_F(utc_blink_cb_form_submit, NonSubmit)
{
  ASSERT_TRUE(ewk_view_url_set( GetEwkWebView(), GetResourceUrl("common/sample_1.html").c_str()));
  EXPECT_EQ(Failure, EventLoopStart(5)); // Wait for timeout
}
