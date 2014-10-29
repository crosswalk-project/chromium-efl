// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef EWK_AUTOFILL_PROFILE_PRIVATE_H_
#define EWK_AUTOFILL_PROFILE_PRIVATE_H_

#include <string>
#include <map>

enum DataType {
  PROFILE_ID = 0,
  PROFILE_NAME,
  PROFILE_COMPANY,
  PROFILE_ADDRESS1,
  PROFILE_ADDRESS2,
  PROFILE_CITY_TOWN,
  PROFILE_STATE_PROVINCE_REGION,
  PROFILE_ZIPCODE,
  PROFILE_COUNTRY,
  PROFILE_PHONE,
  PROFILE_EMAIL,
  MAX_AUTOFILL
};

class _Ewk_Autofill_Profile {
public:
  _Ewk_Autofill_Profile();
  ~_Ewk_Autofill_Profile();

  typedef std::map<DataType, std::string> AutofillDataMap;

  unsigned getProfileID() const;
  std::string getData(DataType name) const;

  void setData(DataType name, const std::string& value);

private:
  static unsigned s_currentID;
  unsigned m_profileID;
  AutofillDataMap m_data;
};

#endif /*EWK_AUTOFILL_PROFILE_PRIVATE_H_*/
