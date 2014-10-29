// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_context_certificate_file_get: public utc_blink_ewk_base
{
};

/**
 * @brief Getting certificate file and comparing with the one which was just set.
 */
TEST_F(utc_blink_ewk_context_certificate_file_get, POS_TEST)
{
  std::string const certificate_file = GetResourcePath("ewk_context/google-cert.pem");
  ewk_context_certificate_file_set(
      ewk_context_default_get(),
      certificate_file.c_str());

  EXPECT_STREQ(certificate_file.c_str(), ewk_context_certificate_file_get(ewk_context_default_get()));
}
