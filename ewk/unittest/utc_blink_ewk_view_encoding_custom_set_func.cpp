// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_view_encoding_custom_set : public utc_blink_ewk_base
{
public:
  static const char* encoding_;
};

const char* utc_blink_ewk_view_encoding_custom_set::encoding_ = "utf-8";

/**
* @brief Check whether function works well with correct webview as parameter
*/
TEST_F(utc_blink_ewk_view_encoding_custom_set, POS_TEST)
{
  ewk_view_encoding_custom_set(GetEwkWebView(), encoding_);
  SUCCEED();
}

/**
* @brief Checking whether function works properly in case of NULL of a webview.
*/
TEST_F(utc_blink_ewk_view_encoding_custom_set, NEG_TEST)
{
  ewk_view_encoding_custom_set(NULL, encoding_);
  SUCCEED();
}
