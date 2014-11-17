// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _PATHS_EFL_H_
#define _PATHS_EFL_H_

namespace base {
class FilePath;
}

class PathsEfl {
 public:
  enum {
    // The value here is taken from chromium/src/chrome/common/chrome_paths.h.
    PATH_START = 1000,
    EDJE_RESOURCE_DIR,
    WEB_DATABSE_DIR,
    DIR_LOCALE,
    PATH_END
  };

  // Register EFL specific path providers, should be called once
  // per each process type.
  static void Register();

 private:
  static bool GetPath(int key, base::FilePath* result);
};

#endif // _PATHS_EFL_H_
