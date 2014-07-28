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
  virtual PersonalDataManager* GetPersonalDataManager() OVERRIDE;
  virtual scoped_refptr<AutofillWebDataService> GetDatabase() OVERRIDE;
  virtual PrefService* GetPrefs() OVERRIDE;
  virtual void HideRequestAutocompleteDialog() OVERRIDE;
  virtual void ShowAutofillSettings() OVERRIDE;
  virtual void ConfirmSaveCreditCard(const AutofillMetrics& metric_logger, const base::Closure& save_card_callback) OVERRIDE;
  virtual void ShowRequestAutocompleteDialog(const FormData& form, const GURL& source_url, const base::Callback<void(const FormStructure*)>& callback) OVERRIDE;
  virtual void ShowAutofillPopup(
      const gfx::RectF& element_bounds,
      base::i18n::TextDirection text_direction,
      const std::vector<base::string16>& values,
      const std::vector<base::string16>& labels,
      const std::vector<base::string16>& icons,
      const std::vector<int>& identifiers,
      base::WeakPtr<AutofillPopupDelegate> delegate) OVERRIDE;
  virtual void UpdateAutofillPopupDataListValues(const std::vector<base::string16>& values, const std::vector<base::string16>& labels) OVERRIDE;
  virtual void HideAutofillPopup() OVERRIDE;
  virtual bool IsAutocompleteEnabled() OVERRIDE;
  virtual void DetectAccountCreationForms(const std::vector<autofill::FormStructure*>& forms) OVERRIDE;
  // content::WebContentsObserver implementation.
  virtual void DidNavigateMainFrame(const content::LoadCommittedDetails& details, const content::FrameNavigateParams& params) OVERRIDE;
  virtual void WebContentsDestroyed(content::WebContents* web_contents) OVERRIDE;
  virtual void WasShown() OVERRIDE;
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

