/*
 * Chromium EFL
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

class utc_blink_ewk_context_web_indexed_database_delete_all : public utc_blink_ewk_base {
protected:
  utc_blink_ewk_context_web_indexed_database_delete_all()
  : utc_blink_ewk_base()
  {
  }

  static void title_changed(void* data, Evas_Object* webview, void* event)
  {
    static_cast<utc_blink_ewk_context_web_indexed_database_delete_all*>(data)->title = ewk_view_title_get(webview);
    static_cast<utc_blink_ewk_context_web_indexed_database_delete_all*>(data)->EventLoopStop(Success);
  }

  virtual void LoadFinished(Evas_Object* webview)
  {
    evas_object_smart_callback_add(GetEwkWebView(), "title,changed", title_changed, this);
  }

  virtual void PostSetUp()
  {
    ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), GetResourceUrl("/ewk_context_web_indexed_database/CreateDB.html").c_str()));
  }

  virtual void PreTearDown()
  {
      evas_object_smart_callback_del(GetEwkWebView(), "title,changed", title_changed);
  }

protected:
  std::string title;
};

/**
* @brief Tests if indexed database is deleted when initiated properly.
*/
TEST_F(utc_blink_ewk_context_web_indexed_database_delete_all, POS_TEST)
{
  ASSERT_EQ(utc_blink_ewk_base::Success, EventLoopStart());
  ASSERT_STREQ("success", utc_blink_ewk_context_web_indexed_database_delete_all::title.c_str());
  ASSERT_EQ(EINA_TRUE, ewk_context_web_indexed_database_delete_all(ewk_view_context_get(GetEwkWebView())));
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), GetResourceUrl("/ewk_context_web_indexed_database/DBExists.html").c_str()));
  ASSERT_EQ(utc_blink_ewk_base::Success, EventLoopStart());
  EXPECT_STREQ("error", utc_blink_ewk_context_web_indexed_database_delete_all::title.c_str());
}

/**
* @brief Tests if indexed db is not deleted when initiated with NULL context.
*/
TEST_F(utc_blink_ewk_context_web_indexed_database_delete_all, NEG_TEST)
{
  ASSERT_EQ(utc_blink_ewk_base::Success, EventLoopStart());
  ASSERT_STREQ("success", utc_blink_ewk_context_web_indexed_database_delete_all::title.c_str());
  ASSERT_NE(EINA_TRUE, ewk_context_web_indexed_database_delete_all(0));
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), GetResourceUrl("/ewk_context_web_indexed_database/DBExists.html").c_str()));
  ASSERT_EQ(utc_blink_ewk_base::Success, EventLoopStart());
  EXPECT_STRNE("error", utc_blink_ewk_context_web_indexed_database_delete_all::title.c_str());
}
