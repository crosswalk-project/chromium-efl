// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_context_web_storage_origins_get: public utc_blink_ewk_base
{
protected:
  utc_blink_ewk_context_web_storage_origins_get()
    : ctx(NULL)
  {
  }

  void LoadFinished(Evas_Object*)
  {
    EventLoopStop(Success);
  }

  void PostSetUp()
  {
    ctx = ewk_view_context_get(GetEwkWebView());
    ASSERT_TRUE(ctx);
  }

  static void origins_get_cb(Eina_List* origins, void* user_data)
  {
    // We don't need origins here
    if (origins) {
      ewk_context_origins_free(origins);
    }

    ASSERT_TRUE(user_data);

    utc_blink_ewk_context_web_storage_origins_get* owner = static_cast<utc_blink_ewk_context_web_storage_origins_get*>(user_data);
    owner->EventLoopStop(Success);
  }

protected:
  Ewk_Context* ctx;
  static const char* const web_storage_sample_url;
};

const char* const utc_blink_ewk_context_web_storage_origins_get::web_storage_sample_url = "http://www.w3schools.com/html/tryit.asp?filename=tryhtml5_webstorage_local";

TEST_F(utc_blink_ewk_context_web_storage_origins_get, callback)
{
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), web_storage_sample_url));
  ASSERT_EQ(Success, EventLoopStart());
  ASSERT_EQ(EINA_TRUE, ewk_context_web_storage_origins_get(ctx, origins_get_cb, this));
  ASSERT_EQ(Success, EventLoopStart());
}

TEST_F(utc_blink_ewk_context_web_storage_origins_get, invalid_args)
{
  ASSERT_EQ(EINA_FALSE, ewk_context_web_storage_origins_get(NULL, NULL, NULL));
  ASSERT_EQ(EINA_FALSE, ewk_context_web_storage_origins_get(ctx, NULL, NULL));
  ASSERT_EQ(EINA_FALSE, ewk_context_web_storage_origins_get(NULL, origins_get_cb, NULL));
}

