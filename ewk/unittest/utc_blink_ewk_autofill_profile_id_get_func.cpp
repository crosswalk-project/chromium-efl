// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
