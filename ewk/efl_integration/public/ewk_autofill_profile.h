/*
 * Copyright (C) 2013 Samsung Electronics
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

/**
 * @file    ewk_autofill_profile.h
 * @brief   Describes the Ewk autofill profile APIs.
 */

#ifndef ewk_autofill_profile_h
#define ewk_autofill_profile_h

#include <Eina.h>

#ifdef __cplusplus
extern "C" {
#endif

enum _Ewk_Autofill_Profile_Data_Type {
  EWK_PROFILE_ID = 0,
  EWK_PROFILE_NAME,
  EWK_PROFILE_COMPANY,
  EWK_PROFILE_ADDRESS1,
  EWK_PROFILE_ADDRESS2,
  EWK_PROFILE_CITY_TOWN,
  EWK_PROFILE_STATE_PROVINCE_REGION,
  EWK_PROFILE_ZIPCODE,
  EWK_PROFILE_COUNTRY,
  EWK_PROFILE_PHONE,
  EWK_PROFILE_EMAIL,
  EWK_MAX_AUTOFILL
};

typedef enum _Ewk_Autofill_Profile_Data_Type Ewk_Autofill_Profile_Data_Type;
typedef struct _Ewk_Autofill_Profile Ewk_Autofill_Profile;

/**
 * Creates a new profile
 *
 * The created profile must be deleted by ewk_autofill_profile_delete
 *
 * @return @c Ewk_Autofill_Profile if new profile is successfully created, @c NULL otherwise
 * @see ewk_autofill_profile_data_set
 * @see ewk_autofill_profile_delete
 */
EAPI Ewk_Autofill_Profile* ewk_autofill_profile_new(void);

/**
 * Deletes a given profile
 *
 * The API will delete the a particular profile only from the memory.
 * To remove the profile permenantly use ewk_context_form_autofill_profile_remove
 *
 * @param profile name
 *
 * @see ewk_autofill_profile_new
 * @see ewk_context_form_autofill_profile_get
 * @see ewk_context_form_autofill_profile_remove
 */
EAPI void ewk_autofill_profile_delete(Ewk_Autofill_Profile* profile);

/**
 * Sets the data in the profile created by ewk_autofill_profile_new
 *
 * The data set by this function is set locally. To save it to database use ewk_context_form_autofill_profile_add
 *
 * @param profile contains the profile data
 * @param name type of attribute to be set
 * @param value value of the attribute
 *
 * @see ewk_autofill_profile_data_get
 * @see Ewk_Autofill_Profile_Data_Type
 * @see ewk_context_form_autofill_profile_add
 */
EAPI void ewk_autofill_profile_data_set(Ewk_Autofill_Profile* profile, Ewk_Autofill_Profile_Data_Type name, const char* value);

/**
 * Gets the id attribute value from a given profile
 *
 * The profile obtained from ewk_context_form_autofill_profile_get will be used to get the profileid
 *
 * @param profile name of profile
 *
 * @return @c Value of attribute (unsigned), @c 0 otherwise
 * @see ewk_autofill_profile_new
 * @see ewk_context_form_autofill_profile_get
 * @see ewk_context_form_autofill_profile_get_all
 */
EAPI unsigned ewk_autofill_profile_id_get(Ewk_Autofill_Profile* profile);

/**
 * Gets the attribute value from a given profile
 *
 * The profile obtained from ewk_context_form_autofill_profile_get will be used to get the data
 *
 * @param profile name of profile
 * @param name name of attribute
 *
 * @return @c Value of attribute (char*), @c NULL otherwise
 * @see ewk_autofill_profile_new
 * @see ewk_context_form_autofill_profile_get
 * @see ewk_context_form_autofill_profile_get_all
 */
EAPI const char* ewk_autofill_profile_data_get(Ewk_Autofill_Profile* profile, Ewk_Autofill_Profile_Data_Type name);

//#endif // TIZEN_WEBKIT2_AUTOFILL_PROFILE_FORM

#ifdef __cplusplus
}
#endif

#endif // ewk_autofill_profile_h

