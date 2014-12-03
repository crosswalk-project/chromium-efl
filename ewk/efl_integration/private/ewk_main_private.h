// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ewk_main_private_h
#define ewk_main_private_h

#include "ewk_export.h"
#include <string>

/**
 * Returns home directory.
 *
 * If home directory is not previously set by ewk_home_directory_set() then
 * the content of $HOME variable is returned.
 * If $HOME is not set then "/tmp" is returned.
 *
 * @return home directory
 */
EAPI const char* ewk_home_directory_get();

#endif // ewk_main_private_h

