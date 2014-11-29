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

class utc_blink_ewk_settings_is_encoding_valid : public utc_blink_ewk_base {
public:
  static const char* supportedEncodings[];
  static const char* unSupportedEncodings[];
};

const char* utc_blink_ewk_settings_is_encoding_valid::supportedEncodings[] = { "UTF-8", "ISO-8859-1", "Windows-1250" };
const char* utc_blink_ewk_settings_is_encoding_valid::unSupportedEncodings[] = { "Sample-Encoding-Name", "FooBar", "", NULL };

/**
* @brief Positive test case of ewk_settings_is_encoding_valid()
*/
TEST_F(utc_blink_ewk_settings_is_encoding_valid, POS_TEST)
{
  size_t numberOfSupportedEncodings = sizeof(supportedEncodings) / sizeof(supportedEncodings[0]);

  for (unsigned i = 0; i < numberOfSupportedEncodings; ++i) {
    ASSERT_EQ(ewk_settings_is_encoding_valid(supportedEncodings[i]), EINA_TRUE);
  }
  SUCCEED();
}


/**
* @brief Negative test case of ewk_settings_is_encoding_valid()
*/
TEST_F(utc_blink_ewk_settings_is_encoding_valid, NEG_TEST)
{
  size_t numberOfUnSupportedEncodings = sizeof(unSupportedEncodings) / sizeof(unSupportedEncodings[0]);

  for (unsigned i = 0; i < numberOfUnSupportedEncodings; ++i) {
    ASSERT_EQ(ewk_settings_is_encoding_valid(unSupportedEncodings[i]), EINA_FALSE);
  }
  SUCCEED();
}
