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
