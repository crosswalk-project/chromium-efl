// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WEB_PROCESS_CONTENT_MAIN_DELEGATE_H
#define WEB_PROCESS_CONTENT_MAIN_DELEGATE_H

#include "base/compiler_specific.h"
#include "content/public/app/content_main.h"
#include "content/public/app/content_main_delegate.h"

namespace content {

class WebProcessContentMainDelegateEfl: public content::ContentMainDelegate {
 public:
  virtual void PreSandboxStartup() override;
  virtual bool BasicStartupComplete(int* exit_code) override;
  virtual ContentRendererClient* CreateContentRendererClient() override;
};

} // namespace content

#endif // WEB_PROCESS_CONTENT_MAIN_DELEGATE_H
