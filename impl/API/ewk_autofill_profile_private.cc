// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ewk_autofill_profile_private.h"

unsigned _Ewk_Autofill_Profile::s_currentID = 0;

_Ewk_Autofill_Profile::_Ewk_Autofill_Profile() {
  m_profileID = s_currentID;
  ++s_currentID;
}

_Ewk_Autofill_Profile::~_Ewk_Autofill_Profile() {
}

unsigned _Ewk_Autofill_Profile::getProfileID() const {
  return m_profileID;
}

std::string _Ewk_Autofill_Profile::getData(
    DataType name) const {
  const AutofillDataMap::const_iterator it = m_data.find(name);

  return (it != m_data.end()) ? (*it).second : std::string();
}

void _Ewk_Autofill_Profile::setData(DataType name,
    const std::string& value) {
  m_data[name] = value;
}
