/*
   Copyright (C) 2014 Samsung Electronics

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef PERSONAL_DATA_MANAGER_FACTORY_H
#define PERSONAL_DATA_MANAGER_FACTORY_H

#ifdef TIZEN_AUTOFILL_SUPPORT
#include "base/strings/string16.h"
#include "base/compiler_specific.h"
#include "base/basictypes.h"
#include "base/memory/ref_counted.h"
#include "base/memory/singleton.h"

template <typename T> struct DefaultSingletonTraits;

namespace autofill {

class PersonalDataManager;
class AutofillProfile;
class CreditCard;

class PersonalDataManagerService {
 public:
  virtual PersonalDataManager* GetPersonalDataManager() = 0;
};

class PersonalDataManagerFactory {
 public:
  // Returns the PersonalDataManager, creating it if it is not
  // yet created.
  PersonalDataManager* GetDataManager();

  static PersonalDataManagerFactory* GetInstance();

  // Returns the number of web and auxiliary profiles.
  int GetProfileCount();

  // Returns the profile with specified |index|
  AutofillProfile* GetProfileByIndex(int index);

 // Adds |profile| to the web database.
  void AddProfile(const AutofillProfile& profile);

  // Updates |profile| which already exists in the web database.
  void UpdateProfile(const AutofillProfile& profile);

  // Removes the profile or credit card represented by |guid|.
  void RemoveByGUID(const std::string& guid);

  // Returns the profile with the specified |guid|, or NULL if there is no
  // profile with the specified |guid|. Both web and auxiliary profiles may
  // be returned.
  AutofillProfile* GetProfileByGUID(const std::string& guid);

  // Returns the number of web and auxiliary CC.
  int GetCreditCardCount();

  // Returns the credit card with specified |index|
  CreditCard* GetCreditCardByIndex(int index);

  // Adds |credit_card| to the web database.
  void AddCreditCard(const CreditCard& credit_card);

  // Updates |credit_card| which already exists in the web database.
  void UpdateCreditCard(const CreditCard& credit_card);

  // Returns the credit card with the specified |guid|, or NULL if there is
  // no credit card with the specified |guid|.
  CreditCard* GetCreditCardByGUID(const std::string& guid);

 private:
  friend struct DefaultSingletonTraits<PersonalDataManagerFactory>;

  PersonalDataManagerFactory();
  virtual ~PersonalDataManagerFactory();

  DISALLOW_COPY_AND_ASSIGN(PersonalDataManagerFactory);
};

}  // namespace autofill
#endif // TIZEN_AUTOFILL_SUPPORT
#endif // PERSONAL_DATA_MANAGER_FACTORY_H
