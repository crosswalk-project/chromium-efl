// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UTC_BLINK_EWK_CONTEXT_FORM_AUTOFILL_PROFILE_UTILS_H
#define UTC_BLINK_EWK_CONTEXT_FORM_AUTOFILL_PROFILE_UTILS_H

#include "utc_blink_ewk_base.h"

const unsigned     TEST_AUTOFILL_PROFILE_ID                    =   12345;
const char* const  TEST_AUTOFILL_PROFILE_NAME                  =   "Mr. Smith";
const char* const  TEST_AUTOFILL_PROFILE_COMPANY               =   "Samsung";
const char* const  TEST_AUTOFILL_PROFILE_ADDRESS1              =   "Existing Street 15";
const char* const  TEST_AUTOFILL_PROFILE_ADDRESS2              =   "NonExisting Street -15";
const char* const  TEST_AUTOFILL_PROFILE_CITY_TOWN             =   "Capitol";
const char* const  TEST_AUTOFILL_PROFILE_STATE_PROVINCE_REGION =   "Beautiful Province";
const char* const  TEST_AUTOFILL_PROFILE_ZIPCODE               =   "12-345";
const char* const  TEST_AUTOFILL_PROFILE_COUNTRY               =   "Neverland";
const char* const  TEST_AUTOFILL_PROFILE_PHONE                 =   "123456789";
const char* const  TEST_AUTOFILL_PROFILE_EMAIL                 =   "someEmail@someServer.com";
const char* const  TEST_AUTOFILL_PROFILE_MAX_AUTOFILL          =   "3";

Ewk_Autofill_Profile* getTestEwkAutofillProfile();

bool checkIfProfileContainsTestData(Ewk_Autofill_Profile *profileToCheck);

#endif //UTC_BLINK_EWK_CONTEXT_FORM_AUTOFILL_PROFILE_UTILS_H
