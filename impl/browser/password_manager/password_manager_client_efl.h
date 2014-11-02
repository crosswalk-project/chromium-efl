// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PASSWORD_MANAGER_CLIENT_EFL_H
#define PASSWORD_MANAGER_CLIENT_EFL_H

#ifdef TIZEN_AUTOFILL_SUPPORT

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "browser/password_manager/content_password_manager_driver.h"
#include "browser/password_manager/password_manager_client.h"
#include "content/public/browser/web_contents_user_data.h"

class PasswordGenerationManager;
class PasswordManager;
class Profile;

namespace content {
class WebContents;
}

// PasswordManagerClientEfl implements the PasswordManagerClient interface.
class PasswordManagerClientEfl
    : public PasswordManagerClient,
      public content::WebContentsUserData<PasswordManagerClientEfl> {
 public:
  virtual ~PasswordManagerClientEfl();

  // PasswordManagerClient implementation.
  virtual void PromptUserToSavePassword(PasswordFormManager* form_to_save) override;
  virtual void PasswordWasAutofilled(const autofill::PasswordFormMap& best_matches) const override;
  virtual void AuthenticateAutofillAndFillForm(scoped_ptr<autofill::PasswordFormFillData> fill_data) override;
  virtual PasswordStore* GetPasswordStore() override;
  virtual PasswordManagerDriver* GetDriver() override;
  virtual base::FieldTrial::Probability GetProbabilityForExperiment(const std::string& experiment_name) override;
  virtual bool IsPasswordSyncEnabled() override;

  bool IsPasswordManagerEnabled();
  void SetPasswordManagerEnabled(bool enabled);

  // Convenience method to allow //chrome code easy access to a PasswordManager
  // from a WebContents instance.
  static PasswordManager* GetManagerFromWebContents(content::WebContents* contents);

  // Convenience method to allow //chrome code easy access to a
  // PasswordGenerationManager from a WebContents instance.
  static PasswordGenerationManager* GetGenerationManagerFromWebContents(content::WebContents* contents);

 private:
  explicit PasswordManagerClientEfl(content::WebContents* web_contents);
  friend class content::WebContentsUserData<PasswordManagerClientEfl>;

  // Callback method to be triggered when authentication is successful for a
  // given password authentication request.  If authentication is disabled or
  // not supported, this will be triggered directly.
  void CommitFillPasswordForm(autofill::PasswordFormFillData* fill_data);

  content::WebContents* web_contents_;
  ContentPasswordManagerDriver driver_;

  // Allows authentication callbacks to be destroyed when this client is gone.
  base::WeakPtrFactory<PasswordManagerClientEfl> weak_factory_;

  DISALLOW_COPY_AND_ASSIGN(PasswordManagerClientEfl);
};

#endif // TIZEN_AUTOFILL_SUPPORT

#endif  // PASSWORD_MANAGER_CLIENT_EFL_H
