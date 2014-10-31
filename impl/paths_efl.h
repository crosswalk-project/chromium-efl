// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _PATHS_EFL_H_
#define _PATHS_EFL_H_

#include "base/base_paths.h"

namespace PathsEfl {
enum {
  // The value here is taken from chromium/src/chrome/common/chrome_paths.h.
  PATH_START = 1000,
  EDJE_RESOURCE_DIR,
  WEB_DATABASE_DIR,
  DIR_USER_DATA,
  DIR_DATA_PATH,
  DIR_LOCALE,
  PATH_END
};

// Call once per each process type to register the provider for the
// path keys defined above
void Register();
}

#endif // _PATHS_EFL_H_
