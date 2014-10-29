// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ewk_certificate.h"

#include <string>

// TODO: remove dependency
#include <../impl/API/ewk_certificate_private.h>


void ewk_certificate_policy_decision_allowed_set(Ewk_Certificate_Policy_Decision* certificatePolicyDecision, Eina_Bool allowed)
{
  EINA_SAFETY_ON_NULL_RETURN(certificatePolicyDecision);
  certificatePolicyDecision->isDecided = true;
  certificatePolicyDecision->callback.Run(allowed);
}

void ewk_certificate_policy_decision_suspend(Ewk_Certificate_Policy_Decision* certificatePolicyDecision)
{
  EINA_SAFETY_ON_NULL_RETURN(certificatePolicyDecision);
  certificatePolicyDecision->isSuspended = true;
}

Eina_Stringshare* ewk_certificate_policy_decision_url_get(Ewk_Certificate_Policy_Decision* certificatePolicyDecision)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(certificatePolicyDecision, 0);
  return certificatePolicyDecision->url;
}

Eina_Stringshare* ewk_certificate_policy_decision_certificate_pem_get(Ewk_Certificate_Policy_Decision* certificatePolicyDecision)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(certificatePolicyDecision, 0);
  return certificatePolicyDecision->certificatePem;
}

int ewk_certificate_policy_decision_error_get(Ewk_Certificate_Policy_Decision* certificatePolicyDecision)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(certificatePolicyDecision, 0);
  return certificatePolicyDecision->error;
}
