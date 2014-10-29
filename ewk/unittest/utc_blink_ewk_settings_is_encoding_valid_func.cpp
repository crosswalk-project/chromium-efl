// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
