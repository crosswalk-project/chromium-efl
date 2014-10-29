// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ewk_autofill_profile.h"

// TODO: remove dependency
#include <../impl/API/ewk_autofill_profile_private.h>

#include "private/ewk_private.h"

Ewk_Autofill_Profile* ewk_autofill_profile_new()
{
  return new(std::nothrow) Ewk_Autofill_Profile();
}

void ewk_autofill_profile_delete(Ewk_Autofill_Profile* profile)
{
  EINA_SAFETY_ON_NULL_RETURN(profile);
  delete profile;
}

void ewk_autofill_profile_data_set(Ewk_Autofill_Profile* profile,
    Ewk_Autofill_Profile_Data_Type name, const char* value)
{
  EINA_SAFETY_ON_NULL_RETURN(profile);
  profile->setData(DataType(name), value);
}

unsigned ewk_autofill_profile_id_get(Ewk_Autofill_Profile* profile)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(profile, 0);
  return profile->getProfileID();
}

const char* ewk_autofill_profile_data_get(Ewk_Autofill_Profile* profile,
    Ewk_Autofill_Profile_Data_Type name)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(profile, 0);
  return (profile->getData(DataType(name)).empty()) ?
          NULL : profile->getData(DataType(name)).c_str();
}
