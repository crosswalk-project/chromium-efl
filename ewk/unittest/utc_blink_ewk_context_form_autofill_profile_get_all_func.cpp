// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"
#include "utc_blink_ewk_context_form_autofill_profile_utils.h"

#include <map>
#include <sstream>

class utc_blink_ewk_context_form_autofill_profile_get_all : public utc_blink_ewk_base
{
};

TEST_F(utc_blink_ewk_context_form_autofill_profile_get_all, POS_TEST)
{
  EventLoopWait(3.0);
  Ewk_Autofill_Profile *profile = ewk_context_form_autofill_profile_get(ewk_context_default_get(), TEST_AUTOFILL_PROFILE_ID);
  if (NULL != profile) {
    ewk_context_form_autofill_profile_remove(ewk_context_default_get(), TEST_AUTOFILL_PROFILE_ID);
  }
  Ewk_Autofill_Profile *profileToAdd = getTestEwkAutofillProfile();
  ASSERT_TRUE(NULL != profileToAdd);
  Eina_Bool result = ewk_context_form_autofill_profile_add(ewk_context_default_get(), profileToAdd);
  EventLoopWait(3.0);
  ASSERT_TRUE(EINA_FALSE != result);
  Eina_List *profilesList = ewk_context_form_autofill_profile_get_all(ewk_context_default_get());
  EventLoopWait(3.0);
  ASSERT_TRUE (NULL != profilesList);
  Eina_List *l;
  void *data;
  profile = NULL;
  EINA_LIST_FOREACH(profilesList, l, data)
  {
    Ewk_Autofill_Profile *tmp_profile = static_cast<Ewk_Autofill_Profile*>(data);
    if (NULL != tmp_profile) {
      if (TEST_AUTOFILL_PROFILE_ID == ewk_autofill_profile_id_get(tmp_profile)) {
        utc_message("found proper profile ID");
        profile = tmp_profile;
      }
    }
  }
  ASSERT_TRUE(NULL == profile);
  ASSERT_TRUE(checkIfProfileContainsTestData(profile));
}


TEST_F(utc_blink_ewk_context_form_autofill_profile_get_all, NEG_TEST)
{
  ASSERT_EQ(NULL, ewk_context_form_autofill_profile_get_all(NULL));
}
