// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifdef TIZEN_AUTOFILL_SUPPORT

#include "browser/password_manager/password_generation_manager.h"

#include "browser/password_manager/password_manager.h"
#include "browser/password_manager/password_manager_client.h"
#include "browser/password_manager/password_manager_driver.h"
#include "components/autofill/content/common/autofill_messages.h"
#include "components/autofill/core/browser/autofill_field.h"
#include "components/autofill/core/browser/field_types.h"
#include "components/autofill/core/browser/form_structure.h"
#include "components/autofill/core/browser/password_generator.h"
#include "components/autofill/core/common/form_data.h"
#include "components/autofill/core/common/password_form.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_contents_view.h"
#include "ui/gfx/rect.h"

PasswordGenerationManager::PasswordGenerationManager(
    content::WebContents* contents,
    PasswordManagerClient* client)
    : web_contents_(contents),
      observer_(NULL),
      client_(client),
      driver_(NULL) {
  if (client)
    driver_ = client->GetDriver();
}

PasswordGenerationManager::~PasswordGenerationManager() {}

void PasswordGenerationManager::SetTestObserver(
    autofill::PasswordGenerationPopupObserver* observer) {
  observer_ = observer;
}

void PasswordGenerationManager::DetectAccountCreationForms(
    const std::vector<autofill::FormStructure*>& forms) {
  std::vector<autofill::FormData> account_creation_forms;
  for (std::vector<autofill::FormStructure*>::const_iterator form_it =
           forms.begin(); form_it != forms.end(); ++form_it) {
    autofill::FormStructure* form = *form_it;
    for (std::vector<autofill::AutofillField*>::const_iterator field_it =
             form->begin(); field_it != form->end(); ++field_it) {
      autofill::AutofillField* field = *field_it;
      if (field->server_type() == autofill::ACCOUNT_CREATION_PASSWORD) {
        account_creation_forms.push_back(form->ToFormData());
        break;
      }
    }
  }
  if (!account_creation_forms.empty() && IsGenerationEnabled()) {
    SendAccountCreationFormsToRenderer(web_contents_->GetRenderViewHost(),
                                       account_creation_forms);
  }
}

// In order for password generation to be enabled, we need to make sure:
// (1) Password sync is enabled, and
// (2) Password saving is enabled.
bool PasswordGenerationManager::IsGenerationEnabled() const
{
  if (!driver_->GetPasswordManager()->IsPasswordManagerSavingEnabled()) {
    DVLOG(2) << "Generation disabled because password saving is disabled";
    return false;
  }

  if (!client_->IsPasswordSyncEnabled()) {
    DVLOG(2) << "Generation disabled because passwords are not being synced";
    return false;
  }

  return true;
}

void PasswordGenerationManager::SendAccountCreationFormsToRenderer(
    content::RenderViewHost* host,
    const std::vector<autofill::FormData>& forms) {
  host->Send(new AutofillMsg_AccountCreationFormsDetected(
      host->GetRoutingID(), forms));
}

gfx::RectF PasswordGenerationManager::GetBoundsInScreenSpace(
    const gfx::RectF& bounds) {
  gfx::Rect client_area;
  web_contents_->GetView()->GetContainerBounds(&client_area);
  return bounds + client_area.OffsetFromOrigin();
}

void PasswordGenerationManager::OnShowPasswordGenerationPopup(
    const gfx::RectF& bounds,
    int max_length,
    const autofill::PasswordForm& form)
{
  // TODO: Call Popup for Password
}

void PasswordGenerationManager::OnShowPasswordEditingPopup(
    const gfx::RectF& bounds,
    const autofill::PasswordForm& form)
{
  // TODO: Call Popup for Edit Password
}

void PasswordGenerationManager::OnHidePasswordGenerationPopup()
{
  HidePopup();
}

void PasswordGenerationManager::HidePopup()
{
  // TODO: Call Popup Hide
}

#endif // TIZEN_AUTOFILL_SUPPORT
