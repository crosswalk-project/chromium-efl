// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ewk_hit_test_private_h
#define ewk_hit_test_private_h

#include <tizen_webview/public/tw_hit_test.h>

struct Ewk_Hit_Test : public tizen_webview::Hit_Test {
};

namespace chromium_glue {
  Ewk_Hit_Test* from(tizen_webview::Hit_Test*);
}

#endif // ewk_hit_test_private_h
