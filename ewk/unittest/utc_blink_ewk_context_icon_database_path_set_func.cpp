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
#include <fstream>
#include <iostream>

class utc_blink_ewk_context_icon_database_path_set : public utc_blink_ewk_base
{
protected:
  void LoadFinished(Evas_Object *) {
    EventLoopStop(Success);
  }

  static bool exists(const char *path) {
    std::ifstream fs(path);
    return fs.good();
  }
};

TEST_F(utc_blink_ewk_context_icon_database_path_set, CORRECT_PATH_SET)
{
  const char *path = "/tmp/utc_blink_favicon_database_test_correct.db";
  ASSERT_EQ(EINA_TRUE, ewk_context_icon_database_path_set(ewk_context_default_get(), path));
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), GetResourceUrl("common/favicon.html").c_str()));
  ASSERT_EQ(Success, EventLoopStart());
  ASSERT_TRUE(exists(path));
}

TEST_F(utc_blink_ewk_context_icon_database_path_set, INCORRECT_PATH_SET)
{
  const char *path = "/tmp/utc_blink_favicon_database_test_incorrect.db";
  ASSERT_EQ(EINA_TRUE, ewk_view_url_set(GetEwkWebView(), GetResourceUrl("common/favicon.html").c_str()));
  ASSERT_EQ(Success, EventLoopStart());
  ASSERT_EQ(EINA_FALSE, ewk_context_icon_database_path_set(ewk_context_default_get(), path));
}
