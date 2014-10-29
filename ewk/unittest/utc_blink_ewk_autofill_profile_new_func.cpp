// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "utc_blink_ewk_base.h"

#include "ewk_autofill_profile.h"

class utc_blink_ewk_autofill_profile_new : public utc_blink_ewk_base
{
};

TEST_F(utc_blink_ewk_autofill_profile_new, POS_TEST)
{
  Ewk_Autofill_Profile* profile = ewk_autofill_profile_new();
  ASSERT_TRUE(profile != NULL);

  ewk_autofill_profile_delete(profile);
}
