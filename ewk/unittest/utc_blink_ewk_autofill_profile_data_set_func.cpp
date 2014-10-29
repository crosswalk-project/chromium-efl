// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string>

#include "utc_blink_ewk_base.h"

#include "ewk_autofill_profile.h"

class utc_blink_ewk_autofill_profile_data_set : public utc_blink_ewk_base
{
protected:
  utc_blink_ewk_autofill_profile_data_set()
  {
    m_profile = ewk_autofill_profile_new();
  }

  virtual ~utc_blink_ewk_autofill_profile_data_set()
  {
    ewk_autofill_profile_delete(m_profile);
  }

  Ewk_Autofill_Profile* m_profile;
};

TEST_F(utc_blink_ewk_autofill_profile_data_set, POS_TEST)
{
  ASSERT_TRUE(m_profile != NULL);
  const std::string m_profileName("MyProfile");

  ewk_autofill_profile_data_set(m_profile, EWK_PROFILE_NAME,
      m_profileName.c_str());

  const std::string result(ewk_autofill_profile_data_get(m_profile,
      EWK_PROFILE_NAME));

  ASSERT_TRUE(result.compare(m_profileName) == 0);
}
