// Copyright (c) 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Defines all the "content" command-line switches only for efl, or Tizen.

#ifndef CONTENT_SWITCHES_EFL_H_
#define CONTENT_SWITCHES_EFL_H_

#include "build/build_config.h"
#include "content/common/content_export.h"

namespace switches {

// Enables the use of view-mode CSS media feature, which allows
// pages to match the different visual presentation modes that can be applied
// to web applications and thereby apply different styling based on these
// different modes using CSS Media Queries.
CONTENT_EXPORT extern const char kEnableViewMode[];

// DON'T ADD RANDOM STUFF HERE. Put it in the main section above in
// alphabetical order, or in one of the ifdefs (also in order in each section).

}  // namespace switches

#endif  // CONTENT_SWITCHES_EFL_H_
