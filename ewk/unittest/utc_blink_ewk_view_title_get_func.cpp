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

#define SAMPLE_HTML_FILE ("/common/sample.html")
#define TITLE0 "Test file"

class utc_blink_ewk_view_title_get_func : public utc_blink_ewk_base
{
protected:
  void LoadFinished(Evas_Object*)
  {
    EventLoopStop(utc_blink_ewk_base::Success); // won't fail the test if EventLoopStop was already called
  }
};

/**
 * @brief Positive test case of ewk_view_title_get
 */
TEST_F(utc_blink_ewk_view_title_get_func, POS_TEST)
{
    Eina_Bool result = ewk_view_url_set(GetEwkWebView(), GetResourceUrl("common/sample.html").c_str());
    if (!result)
        utc_fail();
    if (EventLoopStart() != utc_blink_ewk_base::Success)
        utc_fail();
    char* title = strdup(ewk_view_title_get(GetEwkWebView()));
    utc_message("Current title: %s", title);
    utc_check_str_eq(title, TITLE0);
    free(title);
}

/**
 * @brief Negative test case of ewk_view_title_get
 */
TEST_F(utc_blink_ewk_view_title_get_func, NEG_TEST)
{
  const char* title = ewk_view_title_get(0);
  if (title)
    utc_fail();
}
