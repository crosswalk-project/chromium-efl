/*
 * Copyright (C) 2014 Samsung Electronics
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "ewk_hit_test_private.h"


// Use compiler generated constructor and destructor
// No significant implementation is required.

Ewk_Hit_Test* chromium_glue::from(tizen_webview::Hit_Test* val) {
  // Down-casting to Ewk_Hit_Test is alright because it is the only class
  // that inherits tizen_webview::Hit_Test
  return static_cast<Ewk_Hit_Test*>(val);
}
