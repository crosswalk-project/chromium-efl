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

class utc_blink_ewk_context_web_database_origins_get : public utc_blink_ewk_base {
protected:
  utc_blink_ewk_context_web_database_origins_get()
    : ctx(NULL)
  {
  }

  void PostSetUp()
  {
    utc_message("[postSetUp] :: ");
    ctx = ewk_view_context_get(GetEwkWebView());
  }

  void LoadFinished(Evas_Object* webview)
  {
    utc_message("[loadFinished] :: ");
    EventLoopStop(Success);
  }

  static void origins_get_cb(Eina_List* origins, void* user_data)
  {
    utc_message("origins_get_cb: %p, %p", origins, user_data);
    ASSERT_TRUE(origins);
    ASSERT_TRUE(user_data);
    ASSERT_GE(1, eina_list_count(origins));
    utc_blink_ewk_context_web_database_origins_get* owner = static_cast<utc_blink_ewk_context_web_database_origins_get*>(user_data);
    owner->EventLoopStop(Success);
  }

protected:
  Ewk_Context* ctx;
  static const char* const web_database_file;
};

const char* const utc_blink_ewk_context_web_database_origins_get::web_database_file = "ewk_context/WebDB2.html";

/**
* @brief Tests if can get web database origins after loading a web page.
*/
TEST_F(utc_blink_ewk_context_web_database_origins_get, POS_TEST)
{
  // Load from file
  std::string resurl = GetResourceUrl(web_database_file);
  utc_message("Loading resource: %s", resurl.c_str());
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), resurl.c_str()));
  ASSERT_EQ(Success, EventLoopStart());
  ASSERT_EQ(EINA_TRUE, ewk_context_web_database_origins_get(ctx, origins_get_cb, this));
  ASSERT_EQ(Success, EventLoopStart());
}

/**
* @brief Tests if returns false when context is null.
*/
TEST_F(utc_blink_ewk_context_web_database_origins_get, NEG_TEST1)
{
  ASSERT_EQ(EINA_FALSE, ewk_context_web_database_origins_get(0, origins_get_cb, 0));
}

/**
* @brief Tests if returns false which callback was null.
*/
TEST_F(utc_blink_ewk_context_web_database_origins_get, NEG_TEST2)
{
  ASSERT_EQ(EINA_FALSE, ewk_context_web_database_origins_get(ctx, 0, 0));
}