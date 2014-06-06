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

#ifdef TIZEN_AUTOFILL_SUPPORT

#include "browser/autofill/personal_data_manager_factory.h"

#include "base/memory/scoped_ptr.h"
#include "browser/webdata/web_data_service_factory.h"
#include "components/autofill/core/browser/personal_data_manager.h"
#include "components/autofill/core/browser/webdata/autofill_webdata_service.h"

namespace autofill {
namespace {

class PersonalDataManagerServiceImpl : public PersonalDataManagerService {
 public:
  explicit PersonalDataManagerServiceImpl();
  virtual ~PersonalDataManagerServiceImpl();

  // PersonalDataManagerService:
  virtual void Shutdown() OVERRIDE;
  virtual PersonalDataManager* GetPersonalDataManager() OVERRIDE;
  static PersonalDataManagerServiceImpl* GetInstance() {
    return Singleton<PersonalDataManagerServiceImpl>::get();
  }
 private:
  friend struct DefaultSingletonTraits<PersonalDataManagerServiceImpl>;
  scoped_ptr<PersonalDataManager> personal_data_manager_;
  DISALLOW_COPY_AND_ASSIGN(PersonalDataManagerServiceImpl);
};

PersonalDataManagerServiceImpl::PersonalDataManagerServiceImpl() {
  personal_data_manager_.reset(new PersonalDataManager("en-US"));
  personal_data_manager_->Init(
      WebDataServiceFactory::GetAutofillWebDataForProfile(), false);
}

PersonalDataManagerServiceImpl::~PersonalDataManagerServiceImpl() {
}

void PersonalDataManagerServiceImpl::Shutdown() {
  personal_data_manager_.reset();
}

PersonalDataManager* PersonalDataManagerServiceImpl::GetPersonalDataManager() {
  return personal_data_manager_.get();
}

}  // namespace

// static
PersonalDataManager* PersonalDataManagerFactory::GetDataManager() {
  PersonalDataManagerService* service =
    (PersonalDataManagerService*)(PersonalDataManagerServiceImpl::GetInstance());

  if (service)
    return service->GetPersonalDataManager();

  // |service| can be NULL in Incognito mode.
  return NULL;
}

// static
PersonalDataManagerFactory* PersonalDataManagerFactory::GetInstance() {
  return Singleton<PersonalDataManagerFactory>::get();
}

int PersonalDataManagerFactory::GetProfileCount() {
  return GetDataManager()->GetProfiles().size();
}

AutofillProfile* PersonalDataManagerFactory::GetProfileByIndex(int index) {
  const std::vector<AutofillProfile*>& profiles =
      GetDataManager()->GetProfiles();
  size_t index_size_t = static_cast<size_t>(index);
  DCHECK_LT(index_size_t, profiles.size());
  return (profiles[index_size_t]);
}

void PersonalDataManagerFactory::AddProfile(const AutofillProfile& profile) {
  GetDataManager()->AddProfile(profile);
}

void PersonalDataManagerFactory::UpdateProfile(const AutofillProfile& profile) {
  GetDataManager()->UpdateProfile(profile);
}

void PersonalDataManagerFactory::RemoveByGUID(const std::string& guid) {
  GetDataManager()->RemoveByGUID(guid);
}

AutofillProfile* PersonalDataManagerFactory::GetProfileByGUID(const std::string& guid) {
  return GetDataManager()->GetProfileByGUID(guid);
}

void PersonalDataManagerFactory::AddCreditCard(const CreditCard& credit_card) {
  GetDataManager()->AddCreditCard(credit_card);
}

void PersonalDataManagerFactory::UpdateCreditCard(const CreditCard& credit_card) {
  GetDataManager()->UpdateCreditCard(credit_card);
}

CreditCard* PersonalDataManagerFactory::GetCreditCardByGUID(const std::string& guid) {
  return GetDataManager()->GetCreditCardByGUID(guid);
}

int PersonalDataManagerFactory::GetCreditCardCount() {
  return GetDataManager()->GetCreditCards().size();
}

CreditCard* PersonalDataManagerFactory::GetCreditCardByIndex(int index) {
  const std::vector<CreditCard*>& credit_cards =
      GetDataManager()->GetCreditCards();
  size_t index_size_t = static_cast<size_t>(index);
  DCHECK_LT(index_size_t, credit_cards.size());
  return (credit_cards[index_size_t]);
}

PersonalDataManagerFactory::PersonalDataManagerFactory() {
}

PersonalDataManagerFactory::~PersonalDataManagerFactory() {
}

}  // namespace autofill

#endif // TIZEN_AUTOFILL_SUPPORT
