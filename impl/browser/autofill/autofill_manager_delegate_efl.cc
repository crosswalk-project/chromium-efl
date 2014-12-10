// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifdef TIZEN_AUTOFILL_SUPPORT

#include "browser/autofill/personal_data_manager_factory.h"
#include "browser/webdata/web_data_service_factory.h"
#include "browser/password_manager/password_manager_client_efl.h"
#include "browser/password_manager/password_generation_manager.h"

#include "base/logging.h"
#include "base/prefs/pref_service.h"
#include "base/strings/utf_string_conversions.h"
#include "components/autofill/content/browser/autofill_driver_impl.h"
#include "components/autofill/content/common/autofill_messages.h"
#include "components/autofill/core/common/autofill_pref_names.h"
#include "components/autofill/core/browser/webdata/autofill_webdata_service.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/web_contents_view.h"
#include "content/public/browser/browser_thread.h"
#include "ui/gfx/rect.h"
#include <Elementary.h>
#include "browser/autofill/autofill_manager_delegate_efl.h"

DEFINE_WEB_CONTENTS_USER_DATA_KEY(autofill::AutofillManagerDelegateEfl);

namespace autofill {

AutofillManagerDelegateEfl::AutofillManagerDelegateEfl(content::WebContents* web_contents)
    : content::WebContentsObserver(web_contents)
    , webview_(NULL)
    , web_contents_(web_contents)
    , database_(WebDataServiceFactory::GetInstance()->GetAutofillWebDataForProfile())
    , is_autofill_enabled_(true)
    , popup_controller_(NULL) {
  DCHECK(web_contents);
}

AutofillManagerDelegateEfl::~AutofillManagerDelegateEfl() {
  if (popup_controller_)
    delete popup_controller_;
  popup_controller_ = NULL;
}

void AutofillManagerDelegateEfl::TabActivated() {
  NOTIMPLEMENTED();
}

PersonalDataManager* AutofillManagerDelegateEfl::GetPersonalDataManager() {
  return PersonalDataManagerFactory::GetInstance()->GetDataManager();
}

scoped_refptr<AutofillWebDataService> AutofillManagerDelegateEfl::GetDatabase() {
  return database_;
}

PrefService* AutofillManagerDelegateEfl::GetPrefs() {
  //TODO: Need to implement using Ewk_Settings
  return prefs_.get();
}

void AutofillManagerDelegateEfl::ShowAutofillSettings() {
  NOTIMPLEMENTED();
}

void AutofillManagerDelegateEfl::ConfirmSaveCreditCard(const AutofillMetrics& metric_logger, const base::Closure& save_card_callback) {
  NOTIMPLEMENTED();
}

void AutofillManagerDelegateEfl::ShowRequestAutocompleteDialog(const FormData& form, const GURL& source_url, const base::Callback<void(const FormStructure*)>& callback) {
  HideRequestAutocompleteDialog();
  NOTIMPLEMENTED();
}

void AutofillManagerDelegateEfl::ShowAutofillPopup(
    const gfx::RectF& element_bounds,
    base::i18n::TextDirection text_direction,
    const std::vector<base::string16>& values,
    const std::vector<base::string16>& labels,
    const std::vector<base::string16>& icons,
    const std::vector<int>& identifiers,
    base::WeakPtr<AutofillPopupDelegate> delegate) {
  DCHECK(web_contents_);
#if defined(OS_TIZEN)
  gfx::Rect client_area;
  web_contents_->GetView()->GetContainerBounds(&client_area);
  gfx::RectF element_bounds_in_screen_space = element_bounds + client_area.OffsetFromOrigin();
#endif

  if (GetOrCreatePopupController()) {
    popup_controller_->InitFormData(values,labels,icons,identifiers,delegate);
#if defined(OS_TIZEN)
    popup_controller_->UpdateFormDataPopup(element_bounds_in_screen_space);
#else
    popup_controller_->UpdateFormDataPopup(element_bounds);
#endif
    popup_controller_->Show();
  }
}

void AutofillManagerDelegateEfl::UpdateAutofillPopupDataListValues(const std::vector<base::string16>& values, const std::vector<base::string16>& labels) {
  NOTIMPLEMENTED();
}

void AutofillManagerDelegateEfl::HideAutofillPopup() {
  DCHECK(web_contents_);
  if (popup_controller_) {
    popup_controller_->Hide();
    PasswordGenerationManager * manager = PasswordManagerClientEfl::GetGenerationManagerFromWebContents(web_contents_);
    if (manager)
      manager->HidePopup();
  }
}

bool AutofillManagerDelegateEfl::IsAutocompleteEnabled() {
  return is_autofill_enabled_;
}

void AutofillManagerDelegateEfl::SetAutocompleteEnabled(bool enable) {
  PersonalDataManager * data_manager = GetPersonalDataManager();
  if(data_manager)
    data_manager->SetAutofillEnabled(enable);
  is_autofill_enabled_ = enable;
}

bool AutofillManagerDelegateEfl::IsAutocompleteSavingEnabled() {
  return webview_->GetSettings()->formCandidateData();
}

void AutofillManagerDelegateEfl::HideRequestAutocompleteDialog() {
  NOTIMPLEMENTED();
}

void AutofillManagerDelegateEfl::WasShown() {
  content::RenderViewHost* host = web_contents()->GetRenderViewHost();
  if (host)
    host->Send(new AutofillMsg_PageShown(host->GetRoutingID()));
}

void AutofillManagerDelegateEfl::DidNavigateMainFrame(const content::LoadCommittedDetails& details, const content::FrameNavigateParams& params) {
  HideRequestAutocompleteDialog();
}

void AutofillManagerDelegateEfl::ShowSavePasswordPopup(PasswordFormManager * form_to_save) {
  if (GetOrCreatePopupController())
    popup_controller_->ShowSavePasswordPopup(form_to_save);
}

void AutofillManagerDelegateEfl::WebContentsDestroyed(content::WebContents* web_contents) {
  HideAutofillPopup();
}

void AutofillManagerDelegateEfl::DetectAccountCreationForms(const std::vector<autofill::FormStructure*>& forms) {
  DCHECK(web_contents_);
  PasswordGenerationManager * manager = PasswordManagerClientEfl::GetGenerationManagerFromWebContents(web_contents_);
  if (manager)
    manager->DetectAccountCreationForms(forms);
}

AutofillPopupViewEfl * AutofillManagerDelegateEfl::GetOrCreatePopupController() {
  if (webview_ && !popup_controller_)
    popup_controller_ = new AutofillPopupViewEfl(webview_);
  return popup_controller_;
}

}  // namespace autofill

#endif // TIZEN_AUTOFILL_SUPPORT
