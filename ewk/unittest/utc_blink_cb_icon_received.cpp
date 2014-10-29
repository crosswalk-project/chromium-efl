// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
