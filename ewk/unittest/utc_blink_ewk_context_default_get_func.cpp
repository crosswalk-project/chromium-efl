// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

class utc_blink_ewk_context_default_get : public utc_blink_ewk_base
{
};

/**
 * @brief Checkeing whether a default Ewk_Context is returned.
 */
TEST_F(utc_blink_ewk_context_default_get, POS_TEST)
{
  Eina_Bool result = EINA_FALSE;
  Ewk_Context* context = ewk_context_default_get();
  if (context)
    result = EINA_TRUE;

  utc_check_eq(result, EINA_TRUE);
}

/**
 * @brief There is no negative case for ewk_context_default_get function.
 */
TEST_F(utc_blink_ewk_context_default_get, NEG_TEST)
{
  utc_pass();
}
