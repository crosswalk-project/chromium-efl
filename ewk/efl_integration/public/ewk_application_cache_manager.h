// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/**
 * @file    ewk_application_cache_manager.h
 * @brief   Describes the Ewk Application Cache Manager API.
 */

#ifndef ewk_application_cache_manager_h
#define ewk_application_cache_manager_h

#include <Eina.h>
#include "ewk_export.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Creates a type name for Ewk_Application_Cache_Manager */
typedef struct EwkApplicationCacheManager Ewk_Application_Cache_Manager;

/**
 * Deletes all web application caches.
 *
 * @param manager Ewk_Application_Cache_Manager object
 *
 * @return @c EINA_TRUE on successful request or @c EINA FALSE on failure
 */
EAPI Eina_Bool ewk_application_cache_manager_clear(Ewk_Application_Cache_Manager *manager);

#ifdef __cplusplus
}
#endif

#endif // ewk_application_cache_manager_h
