// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"
#include "utc_blink_ewk_context_form_autofill_profile_utils.h"

#include <map>

class utc_blink_ewk_context_form_autofill_profile_add : public utc_blink_ewk_base
{
};

TEST_F(utc_blink_ewk_context_form_autofill_profile_add, POS_TEST)
{
  Ewk_Autofill_Profile *profile = getTestEwkAutofillProfile();
  EventLoopWait(3.0);
  ASSERT_TRUE(NULL != profile);
  Eina_Bool result = ewk_context_form_autofill_profile_add(ewk_context_default_get(), profile);
  ASSERT_TRUE(EINA_FALSE != result);
}

TEST_F(utc_blink_ewk_context_form_autofill_profile_add, NEG_TEST)
{
  Ewk_Autofill_Profile *profileToAdd = getTestEwkAutofillProfile();
  Ewk_Context* context = ewk_context_default_get();
  ASSERT_TRUE(NULL != profileToAdd);
  ASSERT_TRUE(NULL != context);
  ASSERT_EQ(EINA_FALSE, ewk_context_form_autofill_profile_add(NULL, profileToAdd));
  ASSERT_EQ(EINA_FALSE, ewk_context_form_autofill_profile_add(context, NULL));
}
