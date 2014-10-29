// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ewk_chromium_h
#define ewk_chromium_h

#include "ewk_export.h"

typedef void (*ewk_dispatch_callback)(void *);

/**
 * Call once from the thread you want the dispatcher work on.
 * It should be main thread usually.
 *
 */
EAPI void ewk_dispatcher_touch();

EAPI void ewk_dispatcher_dispatch(ewk_dispatch_callback cb, void *user_data, unsigned delay);

#endif
