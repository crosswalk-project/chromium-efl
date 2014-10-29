// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/**
 * @file    ewk_page_group.h
 * @brief   Describes the Ewk Page Group API.
 */

#ifndef ewk_page_group_h
#define ewk_page_group_h

#include <Eina.h>
#include "ewk_export.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Declare Ewk_Page_Group as Ewk_Object.
 *
 * @see Ewk_Object
 */
typedef struct EwkObject Ewk_Page_Group;

/**
 * Creates a new Ewk_Page_Group.
 *
 * The returned Ewk_Page_Group object @b should be unref'ed after use.
 *
 * @return Ewk_Page_Group object on success or @c NULL on failure
 *
 * @see ewk_object_unref
 */
EAPI Ewk_Page_Group *ewk_page_group_create(const char *identifier);

#ifdef __cplusplus
}
#endif

#endif // ewk_page_group_h
