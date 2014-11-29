/*
   Copyright (C) 2013 Samsung Electronics

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
