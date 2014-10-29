// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifdef TIZEN_AUTOFILL_SUPPORT

#include "browser/autofill/personal_data_manager_factory.h"

#include "eweb_view.h"
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
  personal_data_manager_.reset(new PersonalDataManager(EWebView::GetPlatformLocale()));
  personal_data_manager_->Init(WebDataServiceFactory::GetAutofillWebDataForProfile(), false);
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
  PersonalDataManagerService* service = (PersonalDataManagerService*)(PersonalDataManagerServiceImpl::GetInstance());
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
  if (GetDataManager())
    return GetDataManager()->GetProfiles().size();
  return 0;
}

AutofillProfile* PersonalDataManagerFactory::GetProfileByIndex(int index) {
  if (GetDataManager()) {
    const std::vector<AutofillProfile*>& profiles = GetDataManager()->GetProfiles();
    size_t index_size_t = static_cast<size_t>(index);
    DCHECK_LT(index_size_t, profiles.size());
    return (profiles[index_size_t]);
  }
  return NULL;
}

void PersonalDataManagerFactory::AddProfile(const AutofillProfile& profile) {
  if (GetDataManager())
    GetDataManager()->AddProfile(profile);
}

void PersonalDataManagerFactory::UpdateProfile(const AutofillProfile& profile) {
  if (GetDataManager())
    GetDataManager()->UpdateProfile(profile);
}

void PersonalDataManagerFactory::RemoveByGUID(const std::string& guid) {
  if (GetDataManager())
    GetDataManager()->RemoveByGUID(guid);
}

AutofillProfile* PersonalDataManagerFactory::GetProfileByGUID(const std::string& guid) {
  if (GetDataManager())
    return GetDataManager()->GetProfileByGUID(guid);

  return NULL;
}

void PersonalDataManagerFactory::AddCreditCard(const CreditCard& credit_card) {
  if (GetDataManager())
    GetDataManager()->AddCreditCard(credit_card);
}

void PersonalDataManagerFactory::UpdateCreditCard(const CreditCard& credit_card) {
  if (GetDataManager())
    GetDataManager()->UpdateCreditCard(credit_card);
}

CreditCard* PersonalDataManagerFactory::GetCreditCardByGUID(const std::string& guid) {
  if (GetDataManager())
    return GetDataManager()->GetCreditCardByGUID(guid);

  return NULL;
}

int PersonalDataManagerFactory::GetCreditCardCount() {
  if (GetDataManager())
    return GetDataManager()->GetCreditCards().size();

  return 0;
}

CreditCard* PersonalDataManagerFactory::GetCreditCardByIndex(int index) {
  if (GetDataManager()) {
    const std::vector<CreditCard*>& credit_cards = GetDataManager()->GetCreditCards();
    size_t index_size_t = static_cast<size_t>(index);
    DCHECK_LT(index_size_t, credit_cards.size());
    return (credit_cards[index_size_t]);
  }
  return NULL;
}

PersonalDataManagerFactory::PersonalDataManagerFactory() {
}

PersonalDataManagerFactory::~PersonalDataManagerFactory() {
}

}  // namespace autofill

#endif // TIZEN_AUTOFILL_SUPPORT
