// Copyright 2014 The Chromium Authors. All rights reserved.
// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.


#ifndef CONTENT_PASSWORD_MANAGER_DRIVER_H
#define CONTENT_PASSWORD_MANAGER_DRIVER_H

#ifdef TIZEN_AUTOFILL_SUPPORT

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "browser/password_manager/password_generation_manager.h"
#include "browser/password_manager/password_manager.h"
#include "browser/password_manager/password_manager_driver.h"
#include "content/public/browser/web_contents_observer.h"

namespace autofill {
class AutofillManager;
struct PasswordForm;
}

namespace content {
class WebContents;
}

class ContentPasswordManagerDriver : public PasswordManagerDriver,
                                     public content::WebContentsObserver {
 public:
  explicit ContentPasswordManagerDriver(content::WebContents* web_contents,
                                        PasswordManagerClient* client);
  virtual ~ContentPasswordManagerDriver();

  // PasswordManagerDriver implementation.
  virtual void FillPasswordForm(const autofill::PasswordFormFillData& form_data)
      override;
  virtual bool DidLastPageLoadEncounterSSLErrors() override;
  virtual bool IsOffTheRecord() override;
  virtual PasswordGenerationManager* GetPasswordGenerationManager() override;
  virtual PasswordManager* GetPasswordManager() override;
  virtual autofill::AutofillManager* GetAutofillManager() override;
  virtual void AllowPasswordGenerationForForm(autofill::PasswordForm* form)
      override;

  // content::WebContentsObserver overrides.
  virtual bool OnMessageReceived(const IPC::Message& message) override;
  virtual void DidNavigateMainFrame(
      const content::LoadCommittedDetails& details,
      const content::FrameNavigateParams& params) override;

 private:
  // Must outlive this instance.
  PasswordManagerClient* client_;

  PasswordManager password_manager_;
  PasswordGenerationManager password_generation_manager_;

  DISALLOW_COPY_AND_ASSIGN(ContentPasswordManagerDriver);
};

#endif // TIZEN_AUTOFILL_SUPPORT

#endif  // CONTENT_PASSWORD_MANAGER_DRIVER_H
