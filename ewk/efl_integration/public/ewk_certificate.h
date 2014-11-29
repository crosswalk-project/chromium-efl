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

#ifndef ewk_certificate_h
#define ewk_certificate_h

#include <Eina.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _Ewk_Certificate_Policy_Decision Ewk_Certificate_Policy_Decision;

/**
 * Set the variable to allow the site access about certificate error.
 *
 * @param certificate_policy_decision certificate information data
 *
 * @param allowed decided permission value from user
 */
EAPI void ewk_certificate_policy_decision_allowed_set(Ewk_Certificate_Policy_Decision* certificate_policy_decision, Eina_Bool allowed);

/**
 * Suspend the operation for certificate error policy decision.
 *
 * This suspends the operation for certificate error policy decision when the signal for policy is emitted.
 * This is very usefull to decide the policy from the additional UI operation like the popup.
 *
 * @param certificate_policy_decision certificate information data
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE on failure
 */
EAPI void ewk_certificate_policy_decision_suspend(Ewk_Certificate_Policy_Decision* certificate_policy_decision);

/**
 * Get the variable url to check the site's url data about certificate error.
 *
 * @param certificate_policy_decision certificate information data
 *
 * @return @c url string on success or empty string on failure
 */
EAPI Eina_Stringshare* ewk_certificate_policy_decision_url_get(Ewk_Certificate_Policy_Decision* certificate_policy_decision);

/**
 * Get the variable certificate pem data to check the information about certificate error.
 *
 * @param certificate_policy_decision certificate information data
 *
 * @return @c certificate pem string on success or empty string on failure
 */
EAPI Eina_Stringshare* ewk_certificate_policy_decision_certificate_pem_get(Ewk_Certificate_Policy_Decision* certificate_policy_decision);

/**
 * Get the variable errro number to check the error cause about certificate error.
 *
 * @param certificate_policy_decision certificate information data
 *
 * @return @c error number
 */
EAPI int ewk_certificate_policy_decision_error_get(Ewk_Certificate_Policy_Decision* certificate_policy_decision);

#ifdef __cplusplus
}
#endif
#endif
