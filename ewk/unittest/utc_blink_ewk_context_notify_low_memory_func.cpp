// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_context_notify_low_memory : public utc_blink_ewk_base
{
};

/**
 * @brief Checkeing whether network session is canceled.
 */
TEST_F(utc_blink_ewk_context_notify_low_memory, POS_TEST)
{
  Eina_Bool result = ewk_context_notify_low_memory(ewk_view_context_get(GetEwkWebView()));
  utc_check_eq(result, EINA_TRUE);
}

/**
 * @brief Checking whether function works properly in case of NULL of a context.
 */
TEST_F(utc_blink_ewk_context_notify_low_memory, NEG_TEST)
{
  Eina_Bool result = ewk_context_notify_low_memory(NULL);
  utc_check_ne(result, EINA_TRUE);
}
