// Copyright (c) 2013 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "xwalk_runner_efl.h"

#include <string>
#include <vector>
#include "base/command_line.h"
#include "base/logging.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/common/content_switches.h"
#include "net/base/filename_util.h"
#include "browser_context_efl.h"
#include "browser/resource_dispatcher_host_delegate_efl.h"
#include "content_main_delegate_efl.h"
#include "content_browser_client_efl.h"
#include "web_contents_delegate_efl.h"
#include "xwalk/application/browser/application_system.h"
#include "xwalk/runtime/browser/xwalk_browser_context.h"
#include "xwalk/runtime/browser/xwalk_component.h"
#include "xwalk/runtime/common/xwalk_runtime_features.h"
#include "xwalk/runtime/common/xwalk_switches.h"

namespace xwalk {

namespace {

XWalkRunnerEfl* g_runner_efl = NULL;

// FIXME: Compare with method in startup_browser_creator.cc.
GURL GetURLFromCommandLine(const CommandLine& command_line) {
  const CommandLine::StringVector& args = command_line.GetArgs();

  if (args.empty())
    return GURL();

  GURL url(args[0]);
  if (url.is_valid() && url.has_scheme())
    return url;

  base::FilePath path(args[0]);
  if (!path.IsAbsolute())
    path = MakeAbsoluteFilePath(path);

  return net::FilePathToFileURL(path);
}
}  // namespace

XWalkRunnerEfl::XWalkRunnerEfl() : run_default_message_loop_(true) {
  VLOG(1) << "Creating XWalkRunnerEfl object.";
  DCHECK(!g_runner_efl);
  g_runner_efl = this;

  // Initializing after the g_runner_efl is set to ensure
  // XWalkRunnerEfl::GetInstance() can be used in all sub objects if needed.
  content_browser_client_.reset(new content::ContentBrowserClientEfl());
}

XWalkRunnerEfl::~XWalkRunnerEfl() {
  DCHECK(g_runner_efl);
  g_runner_efl = NULL;
  VLOG(1) << "Destroying XWalkRunnerEfl object.";
}

// static
XWalkRunnerEfl* XWalkRunnerEfl::GetInstance() {
  return g_runner_efl;
}

XWalkRunner* XWalkRunner::GetInstance() {
  return g_runner_efl;
}

void XWalkRunnerEfl::SetBrowserContext(content::BrowserContextEfl* context) {
   browser_context_.reset(context);
}

content::WebContentsDelegateEfl* XWalkRunnerEfl::GetDelegate() {
  return delegate_.get();
}

void XWalkRunnerEfl::SetDelegate(content::WebContentsDelegateEfl * delegate) {
  delegate_.reset(delegate);
}

void XWalkRunnerEfl::PreMainMessageLoopRun() {

  CommandLine* command_line = CommandLine::ForCurrentProcess();
  startup_url_ = GetURLFromCommandLine(*command_line);
  XWalkRunner::CreateComponents();
  if (command_line->HasSwitch(switches::kListFeaturesFlags)) {
    XWalkRuntimeFeatures::GetInstance()->DumpFeaturesFlags();
    run_default_message_loop_ = false;
    return;
  }

#if !defined(SHARED_PROCESS_MODE)
  application::ApplicationSystem* app_sys = app_system();
  app_sys->LaunchFromCommandLine(*command_line, startup_url_,
                                 run_default_message_loop_);
#endif
}

void XWalkRunnerEfl::PostMainMessageLoopRun() {
  DestroyComponents();
  browser_context_.reset();
  delegate_.reset();
}

// static
scoped_ptr<XWalkRunner> XWalkRunner::Create( ) {
  XWalkRunnerEfl * runner = new XWalkRunnerEfl;
  return scoped_ptr<XWalkRunner>(runner);
}

content::ContentBrowserClient* XWalkRunnerEfl::GetContentBrowserClient() {
  return content_browser_client_.get();
}

XWalkBrowserContext* XWalkRunnerEfl::browser_context() {
  return browser_context_.get();
}

}  // namespace xwalk
