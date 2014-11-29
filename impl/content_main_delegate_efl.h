// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_MAIN_DELEGATE
#define CONTENT_MAIN_DELEGATE

#include "content/public/app/content_main_delegate.h"

#include "base/compiler_specific.h"
#include "content/public/browser/content_browser_client.h"
#include "common/content_client_efl.h"

#if defined(XWALK_EFL)
#include "xwalk_runner_efl.h"
#endif

class EWebContext;

namespace content {

class ContentMainDelegateEfl
    : public ContentMainDelegate {
 public:
  ContentMainDelegateEfl() {}
  // Tells the embedder that the absolute basic startup has been done, i.e.
  // it's now safe to create singletons and check the command line. Return true
  // if the process should exit afterwards, and if so, |exit_code| should be
  // set. This is the place for embedder to do the things that must happen at
  // the start. Most of its startup code should be in the methods below.
  virtual bool BasicStartupComplete(int* exit_code) override;
  virtual void PreSandboxStartup() override;
  virtual ContentBrowserClient* CreateContentBrowserClient() override;

 private:
#if defined(XWALK_EFL)
  scoped_ptr<xwalk::XWalkRunner> runner_efl_;
#endif
  scoped_ptr<ContentBrowserClient> client_;
  ContentClientEfl content_client_;
  DISALLOW_COPY_AND_ASSIGN(ContentMainDelegateEfl);
};

}

#endif
