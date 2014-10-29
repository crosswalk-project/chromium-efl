// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ewk_hit_test_private.h"


// Use compiler generated constructor and destructor
// No significant implementation is required.

Ewk_Hit_Test* chromium_glue::from(tizen_webview::Hit_Test* val) {
  // Down-casting to Ewk_Hit_Test is alright because it is the only class
  // that inherits tizen_webview::Hit_Test
  return static_cast<Ewk_Hit_Test*>(val);
}
