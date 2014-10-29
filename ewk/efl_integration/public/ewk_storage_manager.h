// Copyright 2012 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

 /**
  * @file   ewk_storage_manager.h
  * @brief  Describes the Ewk Storage Manager API.
  *
  * Ewk Storage Manager manages web storage.
  */

#ifndef ewk_storage_manager_h
#define ewk_storage_manager_h

#include <Eina.h>
#include "ewk_export.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Creates a type name for Ewk_Storage_Manager. */
typedef struct EwkStorageManager Ewk_Storage_Manager;

/**
 * Deletes all local storage.
 *
 * @param manager Ewk_Storage_Manager object
 *
 * @return @c EINA_TRUE on success, @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_storage_manager_entries_clear(Ewk_Storage_Manager *manager);

#ifdef __cplusplus
}
#endif

#endif // ewk_storage_manager_h
