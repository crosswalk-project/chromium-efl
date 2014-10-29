// copyright (C) 2012 Intel Corporation. All rights reserved.
// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/**
 * @file    ewk_object.h
 * @brief   Describes the Ewk Ref Counted API.
 */

#ifndef ewk_object_h
#define ewk_object_h

#include <Eina.h>
#include "ewk_export.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Creates a type name for Ewk_Object */
typedef struct EwkObject Ewk_Object;

/**
 * Increases the reference count of the given Ewk_Object.
 *
 * @param object the Ewk_Object instance to increase the reference count
 *
 * @return a pointer to the object on success, @c NULL otherwise.
 */
EAPI Ewk_Object *ewk_object_ref(Ewk_Object *object);

/**
 * Decreases the reference count of the given Ewk_Object, possibly freeing it.
 *
 * When the reference count reaches 0, the item is freed.
 *
 * @param object the Ewk_Object instance to decrease the reference count
 */
EAPI void ewk_object_unref(Ewk_Object *object);

#ifdef __cplusplus
}
#endif

#endif // ewk_object_h
