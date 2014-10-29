// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_BROWSER_MAIN_PARTS_EFL_H_
#define CONTENT_BROWSER_MAIN_PARTS_EFL_H_

#include "base/basictypes.h"
#include "base/memory/scoped_ptr.h"
#include "content/public/browser/browser_main_parts.h"

namespace content {

class DevToolsDelegateEfl;

// EFL port implementation of BrowserMainParts.
// This class contains different "stages" to be executed by BrowserMain(),
// Each stage is represented by a single BrowserMainParts method, called from
// the corresponding method in BrowserMainLoop.
class BrowserMainPartsEfl : public BrowserMainParts {
 public:
  explicit BrowserMainPartsEfl();
  virtual ~BrowserMainPartsEfl();

  // BrowserMainParts overrides.
  virtual void PreMainMessageLoopRun() OVERRIDE;
  virtual void PostMainMessageLoopRun() OVERRIDE;

 private:
  DevToolsDelegateEfl* devtools_delegate_;

  DISALLOW_COPY_AND_ASSIGN(BrowserMainPartsEfl);
};

}  // namespace content

#endif  // CONTENT_BROWSER_MAIN_PARTS_EFL_H_
