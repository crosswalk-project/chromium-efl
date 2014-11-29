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
#include "utc_blink_ewk_context_form_autofill_profile_utils.cpp"

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
