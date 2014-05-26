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
