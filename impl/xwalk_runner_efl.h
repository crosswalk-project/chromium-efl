// Copyright (c) 2013 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef XWALK_RUNNER_EFL_H
#define XWALK_RUNNER_EFL_H

#include "base/memory/scoped_ptr.h"
#include "base/memory/scoped_vector.h"
#include "base/values.h"
#include "url/gurl.h"
#include "xwalk/runtime/browser/xwalk_runner.h"

namespace content {
class BrowserContextEfl;
class ContentBrowserClient;
class ContentBrowserClientEfl;
class ContentMainDelegateEfl;
class WebContentsDelegateEfl;
}

namespace xwalk {

class XWalkBrowserContext;

// Main object for the Browser Process execution in Crosswalk. It is created and
// owned by ContentMainDelegateEfl. It's role is to own, setup and teardown all the
// subsystems of Crosswalk.
class XWalkRunnerEfl : public XWalkRunner {
 public:
  // Read the comments below before using this. Relying too much on this
  // accessor makes the code harder to change and harder to reason about.
  static XWalkRunnerEfl* GetInstance();

  virtual ~XWalkRunnerEfl();
  // Stages of main parts. See content/browser_main_parts.h for description.
  virtual void PreMainMessageLoopRun() override;
  virtual void PostMainMessageLoopRun() override;
  virtual content::ContentBrowserClient* GetContentBrowserClient() override;
  virtual XWalkBrowserContext* browser_context() override;

  void SetBrowserContext(content::BrowserContextEfl*);
  content::WebContentsDelegateEfl * GetDelegate();
  void SetDelegate(content::WebContentsDelegateEfl * delegate);

 protected:
  XWalkRunnerEfl();

 private:
  friend class content::ContentMainDelegateEfl;
  friend class content::BrowserContextEfl;

  // This class acts as an "arm" of XWalkRunnerEfl to fulfill Content API needs,
  // it may call us back in some situations where the a more wider view of the
  // objects is necessary, e.g. during render process lifecycle callbacks.
  friend class content::ContentBrowserClientEfl;

  // This class acts as Base class for Runner
  // friend to allow calling derived class constructor from Create method
  friend class XWalkRunner;

  scoped_ptr<content::ContentBrowserClientEfl> content_browser_client_;
  scoped_ptr<content::BrowserContextEfl> browser_context_;
  scoped_ptr<content::WebContentsDelegateEfl> delegate_;

  // Should be about:blank If no URL is specified in command line arguments.
  GURL startup_url_;

  // True if we need to run the default message loop defined in content.
  bool run_default_message_loop_;

  DISALLOW_COPY_AND_ASSIGN(XWalkRunnerEfl);
};

}  // namespace xwalk

#endif  // XWALK_RUNNER_EFL_H
