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
