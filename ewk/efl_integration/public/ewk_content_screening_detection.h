// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ewk_content_screening_detection_h
#define ewk_content_screening_detection_h

#include <Eina.h>
#include "ewk_error.h"
#include "ewk_export.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _Ewk_Content_Screening_Detection Ewk_Content_Screening_Detection;

/**
 * Set the variable to allow the release confirm about malware error.
 *
 * @param content_screening_detection malware information data
 *
 * @param confirmed decided permission value from user
 */
EAPI void ewk_content_screening_detection_confirmed_set(Ewk_Content_Screening_Detection* content_screening_detection, Eina_Bool confirmed);

/**
 * Suspend the operation for content screening detection.
 *
 * This suspends the operation for content screening detection when the signal is emitted.
 * This is very usefull to decide the policy from the additional UI operation like the popup.
 *
 * @param content_screening_detection malware information data
 *
 * @return @c EINA_TRUE on success or @c EINA_FALSE on failure
 */
EAPI void ewk_content_screening_detection_suspend(Ewk_Content_Screening_Detection* content_screening_detection);

/**
 * Get the variable errro structure to check the error cause about malware error.
 *
 * @param content_screening_detection malware information data
 *
 * @return @c error
 */
EAPI Ewk_Error* ewk_content_screening_detection_error_get(Ewk_Content_Screening_Detection* content_screening_detection);

#ifdef __cplusplus
}
#endif
#endif
