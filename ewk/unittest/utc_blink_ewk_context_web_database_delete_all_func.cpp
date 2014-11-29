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

/* Define those macros _before_ you include the utc_blink_ewk.h header file. */

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_context_web_database_delete_all : public utc_blink_ewk_base {
protected:
  utc_blink_ewk_context_web_database_delete_all()
    : ctx(NULL)
    , origins(NULL)
  {
  }

  ~utc_blink_ewk_context_web_database_delete_all()
  {
    SetOrigins(NULL);
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
    ASSERT_TRUE(user_data);
    utc_blink_ewk_context_web_database_delete_all* owner = static_cast<utc_blink_ewk_context_web_database_delete_all*>(user_data);
    owner->SetOrigins(origins);
    owner->EventLoopStop(Success);
  }

  void SetOrigins(Eina_List* new_origins)
  {
    utc_message("[setOrigins] :: ");
    if (new_origins != origins) {
      if (origins) {
        ewk_context_origins_free(origins);
      }

      origins = new_origins;
    }
  }

  void GetOrigins(int expected_count)
  {
    // web database operations are async, we must wait for changes to propagate
    for (int i = 0; i < 3; ++i) {
      if (EINA_TRUE != ewk_context_web_database_origins_get(ctx, origins_get_cb, this)) {
        break;
      }

      if (Success != EventLoopStart()) {
        break;
      }

      if (expected_count == eina_list_count(origins)) {
        break;
      }

      if (!EventLoopWait(3.0)) {
        break;
      }
    }
  }

protected:
  Ewk_Context* ctx;
  Eina_List* origins;
protected:
  static const char* const web_database_file;
};

const char* const utc_blink_ewk_context_web_database_delete_all::web_database_file = "ewk_context/WebDB2.html";

/**
* @brief Tests if can delete all web databases.
*/
TEST_F(utc_blink_ewk_context_web_database_delete_all, POS_TEST)
{
  // Load from file
  std::string resurl = GetResourceUrl(web_database_file);
  utc_message("Loading resource: %s", resurl.c_str());
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), resurl.c_str()));
  ASSERT_EQ(Success, EventLoopStart());

  // Check if any origin is loaded
  ASSERT_GE(1, eina_list_count(origins));

  ASSERT_EQ(EINA_TRUE, ewk_context_web_database_delete_all(ctx));
  GetOrigins(0);
  ASSERT_EQ(0, eina_list_count(origins));

  // if there are no origins in db, the "origins" argument passed to callback is NULL
  // see "void OnGetWebDBOrigins()" in "eweb_context.cc"
  ASSERT_TRUE(origins == NULL);
}

/**
* @brief Tests if returns false when context is null.
*/
TEST_F(utc_blink_ewk_context_web_database_delete_all, NEG_TEST)
{
  ASSERT_EQ(EINA_FALSE, ewk_context_web_database_delete_all(0));
}