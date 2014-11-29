
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

class utc_blink_ewk_view_stop : public utc_blink_ewk_base
{
protected:

  static const char* const sample_html_file;
};

const char* const utc_blink_ewk_view_stop::sample_html_file = "/common/sample.html";

/**
 * @brief Positive  test case of ewk_view_stop()
 * ewk_view_stop should return TRUE  as we are stopping the load with a valid url
 */
TEST_F(utc_blink_ewk_view_stop, POS_TEST)
{
  if (!ewk_view_url_set(GetEwkWebView(), GetResourceUrl(sample_html_file).c_str()))
    FAIL();

  Eina_Bool result = ewk_view_stop(GetEwkWebView());
  EXPECT_EQ(result, EINA_TRUE);
}

/**
 * @brief Negative  test case of ewk_view_stop()
 * ewk_view_stop should return FALSE as we are stopping the load with NULL
 */
TEST_F(utc_blink_ewk_view_stop, NEG_TEST)
{
  Eina_Bool result = ewk_view_stop(NULL);
  EXPECT_EQ(result, EINA_FALSE);
}
