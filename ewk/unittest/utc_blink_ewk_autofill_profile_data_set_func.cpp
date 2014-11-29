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
