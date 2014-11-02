// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#ifndef AUTOFILL_MANAGER_DELEGATE_EFL_H
#define AUTOFILL_MANAGER_DELEGATE_EFL_H

#ifdef TIZEN_AUTOFILL_SUPPORT

#include "eweb_view.h"
#include "base/callback.h"
#include "base/compiler_specific.h"
#include "base/i18n/rtl.h"
#include "base/memory/weak_ptr.h"
#include "components/autofill/core/browser/autofill_manager_delegate.h"
#include "content/public/browser/web_contents_observer.h"
#include "content/public/browser/web_contents_user_data.h"
#include "browser/password_manager/password_form_manager.h"
#include "autofill_popup_view_efl.h"

namespace content {
struct FrameNavigateParams;
struct LoadCommittedDetails;
class WebContents;
}

namespace autofill {

struct FormData;

// Chrome implementation of AutofillManagerDelegate.
class AutofillManagerDelegateEfl
    : public content::WebContentsUserData<AutofillManagerDelegateEfl>,
      public AutofillManagerDelegate,
      public content::WebContentsObserver {
 public:
  virtual ~AutofillManagerDelegateEfl();
  // Called when the tab corresponding to |this| instance is activated.
  void TabActivated();
  // AutofillManagerDelegate implementation.
  virtual PersonalDataManager* GetPersonalDataManager() override;
  virtual scoped_refptr<AutofillWebDataService> GetDatabase() override;
  virtual PrefService* GetPrefs() override;
  virtual void HideRequestAutocompleteDialog() override;
  virtual void ShowAutofillSettings() override;
  virtual void ConfirmSaveCreditCard(const AutofillMetrics& metric_logger, const base::Closure& save_card_callback) override;
  virtual void ShowRequestAutocompleteDialog(const FormData& form, const GURL& source_url, const base::Callback<void(const FormStructure*)>& callback) override;
  virtual void ShowAutofillPopup(
      const gfx::RectF& element_bounds,
      base::i18n::TextDirection text_direction,
      const std::vector<base::string16>& values,
      const std::vector<base::string16>& labels,
      const std::vector<base::string16>& icons,
      const std::vector<int>& identifiers,
      base::WeakPtr<AutofillPopupDelegate> delegate) override;
  virtual void UpdateAutofillPopupDataListValues(const std::vector<base::string16>& values, const std::vector<base::string16>& labels) override;
  virtual void HideAutofillPopup() override;
  virtual bool IsAutocompleteEnabled() override;
  virtual void DetectAccountCreationForms(const std::vector<autofill::FormStructure*>& forms) override;
  // content::WebContentsObserver implementation.
  virtual void DidNavigateMainFrame(const content::LoadCommittedDetails& details, const content::FrameNavigateParams& params) override;
  virtual void WebContentsDestroyed(content::WebContents* web_contents) override;
  virtual void WasShown() override;
  void SetAutocompleteEnabled(bool enable);
  void ShowSavePasswordPopup(PasswordFormManager * form_to_save);
  void SetEWebView(EWebView * view) { webview_ = view; }
  AutofillPopupViewEfl * GetOrCreatePopupController();

 private:
  explicit AutofillManagerDelegateEfl(content::WebContents* web_contents);
  friend class content::WebContentsUserData<AutofillManagerDelegateEfl>;

 private:
  content::WebContents* const web_contents_;
  EWebView * webview_;
  scoped_refptr<AutofillWebDataService> database_;
  scoped_ptr<PrefService> prefs_;
  bool is_autofill_enabled_;
  AutofillPopupViewEfl * popup_controller_;

  DISALLOW_COPY_AND_ASSIGN(AutofillManagerDelegateEfl);
};

}  // namespace autofill

#endif // TIZEN_AUTOFILL_SUPPORT

#endif  // AUTOFILL_MANAGER_DELEGATE_EFL_H

