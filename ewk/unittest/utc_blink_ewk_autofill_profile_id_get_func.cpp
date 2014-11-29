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

#include "ewk_autofill_profile.h"

class utc_blink_ewk_autofill_profile_id_get : public utc_blink_ewk_base
{
protected:
  utc_blink_ewk_autofill_profile_id_get()
  {
    m_profile = ewk_autofill_profile_new();
  }

  virtual ~utc_blink_ewk_autofill_profile_id_get()
  {
    ewk_autofill_profile_delete(m_profile);
  }

  Ewk_Autofill_Profile* m_profile;
};

TEST_F(utc_blink_ewk_autofill_profile_id_get, POS_TEST)
{
  unsigned id = ewk_autofill_profile_id_get(m_profile);
  /* There is no way to set value of id. Default value set by constructor
   * is 0 and the only way to test ewk_autofill_profile_id_get is check if
   * returns this default value.
   */
  ASSERT_EQ(0, id);
}
