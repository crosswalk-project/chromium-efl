// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
