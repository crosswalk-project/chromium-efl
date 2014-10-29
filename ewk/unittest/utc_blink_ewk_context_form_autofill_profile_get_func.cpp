// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"
#include "utc_blink_ewk_context_form_autofill_profile_utils.h"

#include <map>
#include <sstream>

class utc_blink_ewk_context_form_autofill_profile_get : public utc_blink_ewk_base
{
};

TEST_F(utc_blink_ewk_context_form_autofill_profile_get, POS_TEST)
{
  EventLoopWait(3.0);
  Ewk_Autofill_Profile *profile = ewk_context_form_autofill_profile_get(ewk_context_default_get(), TEST_AUTOFILL_PROFILE_ID);
  if (NULL == profile) {
    Ewk_Autofill_Profile *profileToAdd = getTestEwkAutofillProfile();
    ASSERT_TRUE(NULL != profileToAdd);
    Eina_Bool result = ewk_context_form_autofill_profile_add(ewk_context_default_get(), profileToAdd);
    EventLoopWait(3.0);
    ASSERT_TRUE(EINA_FALSE != result);
    profile = ewk_context_form_autofill_profile_get(ewk_context_default_get(), TEST_AUTOFILL_PROFILE_ID);
    EventLoopWait(3.0);
    ASSERT_TRUE(NULL != profile);
    ASSERT_TRUE(checkIfProfileContainsTestData(profile));
  } else {
    ASSERT_TRUE(checkIfProfileContainsTestData(profile));
  }
}

TEST_F(utc_blink_ewk_context_form_autofill_profile_get, NEG_TEST)
{
  ASSERT_EQ(NULL, ewk_context_form_autofill_profile_get(NULL, TEST_AUTOFILL_PROFILE_ID));
}
