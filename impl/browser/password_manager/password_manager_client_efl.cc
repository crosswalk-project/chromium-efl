// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifdef TIZEN_AUTOFILL_SUPPORT

#include "browser/password_manager/password_manager_client_efl.h"
#include "browser/autofill/autofill_manager_delegate_efl.h"
#include "autofill_popup_view_efl.h"
#include "base/bind_helpers.h"
#include "base/command_line.h"
#include "base/memory/singleton.h"
#include "base/metrics/histogram.h"
#include "browser/password_manager/password_form_manager.h"
#include "browser/password_manager/password_manager.h"
#include "browser/password_manager/password_manager_util.h"
#include "browser/password_manager/password_store_factory.h"
#include "components/password_manager/core/browser/password_manager_metrics_util.h"
#include "content/public/browser/web_contents.h"

DEFINE_WEB_CONTENTS_USER_DATA_KEY(PasswordManagerClientEfl);

PasswordManagerClientEfl::PasswordManagerClientEfl(content::WebContents* web_contents)
    : web_contents_(web_contents)
    , driver_(web_contents, this)
    , weak_factory_(this) {
}

PasswordManagerClientEfl::~PasswordManagerClientEfl()
{
}

void PasswordManagerClientEfl::PromptUserToSavePassword(PasswordFormManager* form_to_save)
{
  autofill::AutofillManagerDelegateEfl * autofill_manager = autofill::AutofillManagerDelegateEfl::FromWebContents(web_contents_);
  if (autofill_manager) {
    autofill_manager->ShowSavePasswordPopup(form_to_save);
  }
}

void PasswordManagerClientEfl::PasswordWasAutofilled(const autofill::PasswordFormMap& best_matches) const
{
}

void PasswordManagerClientEfl::AuthenticateAutofillAndFillForm(scoped_ptr<autofill::PasswordFormFillData> fill_data)
{
  CommitFillPasswordForm(fill_data.get());
}

PasswordStore* PasswordManagerClientEfl::GetPasswordStore()
{
  return PasswordStoreFactory::GetPasswordStore().get();
}

PasswordManagerDriver* PasswordManagerClientEfl::GetDriver()
{
  return &driver_;
}

base::FieldTrial::Probability PasswordManagerClientEfl::GetProbabilityForExperiment(const std::string& experiment_name)
{
  base::FieldTrial::Probability enabled_probability = 0;
  return enabled_probability;
}

bool PasswordManagerClientEfl::IsPasswordSyncEnabled()
{
  return false;
}

bool PasswordManagerClientEfl::IsPasswordManagerEnabled()
{
  return GetDriver()->GetPasswordManager()->IsPasswordManagerEnabled();
}

void PasswordManagerClientEfl::SetPasswordManagerEnabled(bool enabled)
{
  GetDriver()->GetPasswordManager()->SetPasswordManagerEnabled(enabled);
}

// static
PasswordGenerationManager* PasswordManagerClientEfl::GetGenerationManagerFromWebContents(content::WebContents* contents)
{
  PasswordManagerClientEfl* client = PasswordManagerClientEfl::FromWebContents(contents);
  if (!client) {
    return NULL;
  }
  return client->GetDriver()->GetPasswordGenerationManager();
}

// static
PasswordManager* PasswordManagerClientEfl::GetManagerFromWebContents(content::WebContents* contents)
{
  PasswordManagerClientEfl* client = PasswordManagerClientEfl::FromWebContents(contents);
  if (!client) {
    return NULL;
  }
  return client->GetDriver()->GetPasswordManager();
}

void PasswordManagerClientEfl::CommitFillPasswordForm(autofill::PasswordFormFillData* data)
{
  driver_.FillPasswordForm(*data);
}
#endif // TIZEN_AUTOFILL_SUPPORT
