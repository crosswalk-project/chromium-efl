// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_cb_undo_size : public utc_blink_ewk_base
{
 protected:

  utc_blink_cb_undo_size() : size(0)
  {
  }

  void PostSetUp()
  {
    evas_object_smart_callback_add(GetEwkWebView(), "undo,size", ToSmartCallback(undo_size), this);
  }

  void PreTearDown()
  {
    evas_object_smart_callback_del(GetEwkWebView(), "undo,size", ToSmartCallback(undo_size));
  }

  void LoadFinished(Evas_Object*)
  {
    EventLoopStop(Success);
  }

  static void undo_size(utc_blink_cb_undo_size* owner, Evas_Object*, unsigned long* size)
  {
    ASSERT_TRUE(owner);
    utc_message("\e[0;32m[ %s ]\e[0m", __FUNCTION__);
    if (size) owner->size=*size;
    owner->EventLoopStop(Success);
  }

protected:
  unsigned long size;
};

TEST_F(utc_blink_cb_undo_size, POS_TEST)
{
  ASSERT_TRUE(ewk_view_url_set(GetEwkWebView(), GetResourceUrl("ewk_view/undo_redo.html").c_str()));
  // Wait for LoadFinished
  ASSERT_EQ(Success, EventLoopStart());

  for (int i = 4; i > 0; --i) {
    utc_message("edit");
    ewk_view_command_execute(GetEwkWebView(),"SelectAll", NULL);
    ewk_view_command_execute(GetEwkWebView(),"InsertText", ".");
    // Wait for undo_size callback
    ASSERT_EQ(Success, EventLoopStart());
  }
  ASSERT_EQ(3, size);

  utc_message("Undo");
  ewk_view_command_execute(GetEwkWebView(),"Undo", NULL);
  // Wait for undo_size callback
  ASSERT_EQ(Success, EventLoopStart());
  ASSERT_EQ(2, size);

  utc_message("Undo");
  ewk_view_command_execute(GetEwkWebView(),"Undo", NULL);
  // Wait for undo_size callback
  ASSERT_EQ(Success, EventLoopStart());
  ASSERT_EQ(1, size);

  utc_message("Redo");
  ewk_view_command_execute(GetEwkWebView(),"Redo", NULL);
  // Wait for undo_size callback
  EXPECT_EQ(Success, EventLoopStart());
  ASSERT_EQ(2, size);
}
