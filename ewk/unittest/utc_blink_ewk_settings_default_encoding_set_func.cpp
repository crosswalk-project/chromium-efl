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

class utc_blink_ewk_settings_default_encoding_set : public utc_blink_ewk_base {
public:
  static const char* SET_DEFAULT_ENCODING;
};

const char* utc_blink_ewk_settings_default_encoding_set::SET_DEFAULT_ENCODING = "utf-8";


/**
 * @brief Positive test case for ewk_settings_default_encoding_set(). Set the encoding, get the value and then compare.
 */
TEST_F(utc_blink_ewk_settings_default_encoding_set, POS_TEST)
{
  Ewk_Settings* settings = ewk_view_settings_get(GetEwkWebView());
  if (!settings) {
    FAIL();
  }

  Eina_Bool result = ewk_settings_default_encoding_set(settings, SET_DEFAULT_ENCODING);
  if (!result) {
    FAIL();
  }
  const char* defaultEncoding = ewk_settings_default_encoding_get(settings);
  if (!defaultEncoding) {
    FAIL();
  }
  EXPECT_STREQ(defaultEncoding, SET_DEFAULT_ENCODING);
}

/**
 * @brief Negative test case for ewk_settings_default_encoding_set(). Check if it works fine with NULL as settings.
 */
TEST_F(utc_blink_ewk_settings_default_encoding_set, NEG_TEST)
{
  Eina_Bool result = ewk_settings_default_encoding_set(NULL, SET_DEFAULT_ENCODING);
  EXPECT_EQ(result, EINA_FALSE);
}
