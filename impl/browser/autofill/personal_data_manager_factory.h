// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
