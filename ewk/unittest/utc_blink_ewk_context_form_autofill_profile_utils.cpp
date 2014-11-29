#include "utc_blink_ewk_context_form_autofill_profile_utils.h"
#include "utc_blink_ewk_base.h"
#include "ewk_autofill_profile.h"
#include <sstream>
#include <stdio.h>
#include <iostream>

Ewk_Autofill_Profile* getTestEwkAutofillProfile()
{
  Ewk_Autofill_Profile *profile = ewk_autofill_profile_new();
  if (NULL == profile)
    return profile;
  std::stringstream stream;
  stream << TEST_AUTOFILL_PROFILE_ID;
  std::string id = stream.str();
  ewk_autofill_profile_data_set(profile, EWK_PROFILE_ID, id.c_str());
  ewk_autofill_profile_data_set(profile, EWK_PROFILE_NAME, TEST_AUTOFILL_PROFILE_NAME);
  ewk_autofill_profile_data_set(profile, EWK_PROFILE_COMPANY, TEST_AUTOFILL_PROFILE_COMPANY);
  ewk_autofill_profile_data_set(profile, EWK_PROFILE_ADDRESS1, TEST_AUTOFILL_PROFILE_ADDRESS1);
  ewk_autofill_profile_data_set(profile, EWK_PROFILE_ADDRESS2, TEST_AUTOFILL_PROFILE_ADDRESS2);
  ewk_autofill_profile_data_set(profile, EWK_PROFILE_CITY_TOWN, TEST_AUTOFILL_PROFILE_CITY_TOWN);
  ewk_autofill_profile_data_set(profile, EWK_PROFILE_STATE_PROVINCE_REGION, TEST_AUTOFILL_PROFILE_STATE_PROVINCE_REGION);
  ewk_autofill_profile_data_set(profile, EWK_PROFILE_ZIPCODE, TEST_AUTOFILL_PROFILE_ZIPCODE);
  ewk_autofill_profile_data_set(profile, EWK_PROFILE_COUNTRY, TEST_AUTOFILL_PROFILE_COUNTRY);
  ewk_autofill_profile_data_set(profile, EWK_PROFILE_PHONE, TEST_AUTOFILL_PROFILE_PHONE);
  ewk_autofill_profile_data_set(profile, EWK_PROFILE_EMAIL, TEST_AUTOFILL_PROFILE_EMAIL);
  ewk_autofill_profile_data_set(profile, EWK_MAX_AUTOFILL, TEST_AUTOFILL_PROFILE_MAX_AUTOFILL);
  return profile;
};


bool checkOne(Ewk_Autofill_Profile *profileToCheck, const char* reference, Ewk_Autofill_Profile_Data_Type kind, const char* errMessage) {
  const char *ptr = ewk_autofill_profile_data_get(profileToCheck, kind);
  if (NULL == ptr) {
    utc_message("%s, expected: %s obtained: NULL\n", errMessage, reference);
    return false;
  } else {
    utc_message("%s, expected: %s obtained: %s\n", errMessage, reference, ptr);
  }
  return (0 == strcmp(reference, ptr));
}

bool checkIfProfileContainsTestData(Ewk_Autofill_Profile *profileToCheck) {
  if (NULL == profileToCheck) {
    return false;
  }
  bool dataValid = true;
  if (!TEST_AUTOFILL_PROFILE_ID == ewk_autofill_profile_id_get(profileToCheck)) {
    utc_message("profile ID check failed %i %i", TEST_AUTOFILL_PROFILE_ID, ewk_autofill_profile_id_get(profileToCheck));
    dataValid = false;
  }
  dataValid = checkOne(profileToCheck, TEST_AUTOFILL_PROFILE_NAME, EWK_PROFILE_NAME, "name") && dataValid;
  dataValid = checkOne(profileToCheck, TEST_AUTOFILL_PROFILE_COMPANY, EWK_PROFILE_COMPANY, "company") && dataValid;
  dataValid = checkOne(profileToCheck, TEST_AUTOFILL_PROFILE_ADDRESS1, EWK_PROFILE_ADDRESS1, "address1") && dataValid;
  dataValid = checkOne(profileToCheck, TEST_AUTOFILL_PROFILE_ADDRESS2, EWK_PROFILE_ADDRESS2, "address2") && dataValid;
  dataValid = checkOne(profileToCheck, TEST_AUTOFILL_PROFILE_CITY_TOWN, EWK_PROFILE_CITY_TOWN, "city") && dataValid;
  dataValid = checkOne(profileToCheck, TEST_AUTOFILL_PROFILE_STATE_PROVINCE_REGION, EWK_PROFILE_STATE_PROVINCE_REGION, "province") && dataValid;
  dataValid = checkOne(profileToCheck, TEST_AUTOFILL_PROFILE_ZIPCODE, EWK_PROFILE_ZIPCODE, "zipcode") && dataValid;
  dataValid = checkOne(profileToCheck, TEST_AUTOFILL_PROFILE_COUNTRY, EWK_PROFILE_COUNTRY, "country") && dataValid;
  dataValid = checkOne(profileToCheck, TEST_AUTOFILL_PROFILE_PHONE, EWK_PROFILE_PHONE, "phone") && dataValid;
  dataValid = checkOne(profileToCheck, TEST_AUTOFILL_PROFILE_EMAIL, EWK_PROFILE_EMAIL, "email") && dataValid;
  dataValid = checkOne(profileToCheck, TEST_AUTOFILL_PROFILE_MAX_AUTOFILL, EWK_MAX_AUTOFILL, "max_autofill") && dataValid;
  return dataValid;
};
