// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_context_certificate_file_set: public utc_blink_ewk_base
{
};

/**
 * @brief Setting certificate file.
 */
TEST_F(utc_blink_ewk_context_certificate_file_set, POS_TEST)
{
  bool result = ewk_context_certificate_file_set(
      ewk_context_default_get(),
      GetResourcePath("ewk_context/google-cert.pem").c_str());

  utc_check_true(result);
}

TEST_F(utc_blink_ewk_context_certificate_file_set, NEG_TEST)
{
  bool result = ewk_context_certificate_file_set(NULL, NULL);

  utc_check_false(result);
}
