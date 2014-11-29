/*
 * Copyright (C) 2014 Samsung Electronics
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

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
