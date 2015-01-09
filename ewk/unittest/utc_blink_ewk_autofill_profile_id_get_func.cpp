// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"
#include "ewk_autofill_profile.h"

class utc_blink_ewk_autofill_profile_id_get : public utc_blink_ewk_base
{
};

TEST_F(utc_blink_ewk_autofill_profile_id_get, NullArg)
{
  ASSERT_EQ(0, ewk_autofill_profile_id_get(NULL));
}

TEST_F(utc_blink_ewk_autofill_profile_id_get, POS_TEST)
{
  Ewk_Autofill_Profile* const m_profile0 = ewk_autofill_profile_new();
  Ewk_Autofill_Profile* const m_profile1 = ewk_autofill_profile_new();

  EXPECT_NE(ewk_autofill_profile_id_get(m_profile0), ewk_autofill_profile_id_get(m_profile1));
  EXPECT_NE(0, ewk_autofill_profile_id_get(m_profile1));

  ewk_autofill_profile_delete(m_profile1);
  ewk_autofill_profile_delete(m_profile0);
}
