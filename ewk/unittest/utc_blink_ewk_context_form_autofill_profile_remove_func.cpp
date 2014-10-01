/*
 * chromium EFL
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
#include "utc_blink_ewk_context_form_autofill_profile_utils.h"

#include <map>
#include <sstream>

class utc_blink_ewk_context_form_autofill_profile_remove : public utc_blink_ewk_base
{
};

TEST_F(utc_blink_ewk_context_form_autofill_profile_remove, POS_TEST)
{
  EventLoopWait(3.0);
  Ewk_Autofill_Profile *profile = ewk_context_form_autofill_profile_get(ewk_context_default_get(), TEST_AUTOFILL_PROFILE_ID);
  if (NULL == profile) {
    Ewk_Autofill_Profile *profileToAdd = getTestEwkAutofillProfile();
    ASSERT_TRUE(NULL != profileToAdd);
    Eina_Bool result = ewk_context_form_autofill_profile_add(ewk_context_default_get(), profileToAdd);
    ASSERT_TRUE(EINA_FALSE != result);
    EventLoopWait(3.0);
    profile = ewk_context_form_autofill_profile_get(ewk_context_default_get(), TEST_AUTOFILL_PROFILE_ID);
  }
  ASSERT_TRUE(NULL != profile);
  ASSERT_EQ(EINA_TRUE,ewk_context_form_autofill_profile_remove(ewk_context_default_get(), TEST_AUTOFILL_PROFILE_ID));
  EventLoopWait(3.0);
  profile = ewk_context_form_autofill_profile_get(ewk_context_default_get(), TEST_AUTOFILL_PROFILE_ID);
  ASSERT_TRUE(NULL == profile);
}

TEST_F(utc_blink_ewk_context_form_autofill_profile_remove, NEG_TEST)
{
  ASSERT_EQ(EINA_FALSE, ewk_context_form_autofill_profile_remove(NULL, TEST_AUTOFILL_PROFILE_ID));
}
