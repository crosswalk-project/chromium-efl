// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "command_line_efl.h"

#include <string>
#include <cstring>
#include <sys/types.h>
#include <unistd.h>

#include "content/public/common/content_switches.h"
#include "content/public/common/content_client.h"
#include "cc/base/switches.h"
#include "ui/events/event_switches.h"
#include "extensions/common/switches.h"
#include "ui/gl/gl_switches.h"
#include "url/gurl.h"


int CommandLineEfl::argc_ = 0;
char** CommandLineEfl::argv_ = NULL;
CommandLineEfl::ArgumentVector CommandLineEfl::original_arguments_;
bool CommandLineEfl::is_initialized_ = false;

void CommandLineEfl::Init(int argc, char *argv[]) {
  if (CommandLineEfl::is_initialized_) {
    LOG(ERROR) << "CommandLineEfl::Init should not be called more than once";
  }
  CommandLine::Init(argc, argv);
  argc_ = argc;
  argv_ = argv;

  // Unfortunately chromium modifies application's argument vector.
  // during initialization. This means that chromium after initialization
  // argv will only contain one valid entry, argv[0]. To be able to use
  // user provided arguments after initialization we need to make a copy
  // of them.
  // See: chromium/src/content/common/set_process_title_linux.cc
  for (int i = 0; i < argc; ++i)
    original_arguments_.push_back(std::string(argv[i]));
  CommandLineEfl::is_initialized_ = true;
}

content::MainFunctionParams CommandLineEfl::GetDefaultPortParams() {
  if (!CommandLineEfl::is_initialized_) {
    CommandLineEfl::Init(0, NULL);
  }
  CommandLine* p_command_line = CommandLine::ForCurrentProcess();

  p_command_line->AppendSwitch(switches::kNoSandbox);
  p_command_line->AppendSwitch(switches::kDisablePlugins);
  p_command_line->AppendSwitch(switches::kInProcessGPU);
  p_command_line->AppendSwitchASCII(switches::kUseGL, gfx::kGLImplementationEGLName);
  p_command_line->AppendSwitch(switches::kDisableDelegatedRenderer);

  // (prashant.n): New mechanism supports drawing frame to mailbox only.
  p_command_line->AppendSwitch(cc::switches::kCompositeToMailbox);
  // Enables glFinish call
  p_command_line->AppendSwitch(switches::kWaitForFrameComplete);

#if defined(TIZEN_MULTIMEDIA_SUPPORT) && defined(ENABLE_WEBRTC)
  p_command_line->AppendSwitch(switches::kEnableWebRtcHWH264Encoding);
#endif

#if defined(OS_TIZEN)
  p_command_line->AppendSwitch(switches::kEnableOverscrollNotifications);
#if !defined(EWK_BRINGUP)
  p_command_line->AppendSwitch(switches::kEnableGestureTapHighlight);
#endif
  p_command_line->AppendSwitch(switches::kEnableSpatialNavigation);
  p_command_line->AppendSwitch(switches::kMainFrameResizesAreOrientationChanges);

  p_command_line->AppendSwitch(switches::kForceAccelerated2dCanvas);
#else
  p_command_line->AppendSwitch(switches::kIgnoreGpuBlacklist);
#endif

#if defined(OS_TIZEN_MOBILE)
  p_command_line->AppendSwitch(switches::kTouchEvents);
  p_command_line->AppendSwitch(switches::kEnablePinch);
  p_command_line->AppendSwitch(switches::kUseMobileUserAgent);
  p_command_line->AppendSwitch(switches::kEnableViewportMeta);
#endif

#warning "[M37] Investigae removed command line switches, are they still needed, do they have a replacement?"
  // [M37] Note: The commit "Temporarily disable zero copy as it causes browser crash during regression"
  // is to deprecate kEnableMapImage option.
  // But it was already deprecated during fixing M37 build as no command line option with such name (see above comment)
  // TODO: remove this commit if it turn out the option is unnecessary
  //Disabling temporarily, as it causes browser crash ID:335 in regression
  //p_command_line->AppendSwitch(cc::switches::kEnableMapImage);

  // Threaded compositing breaks touch events. Seems to be a linux specific issue
  // (see: http://code.google.com/p/chromium/issues/detail?id=271791)
  //p_command_line->AppendSwitch(switches::kEnableThreadedCompositing);

#warning "[M37] Investigae removed command line switches, are they still needed, do they have a replacement?"
  //p_command_line->AppendSwitch(switches::kAllowWebUICompositing);

  // XXX: Skia benchmarking should be only used for testing,
  // when enabled the following warning is printed to stderr:
  // "Enabling unsafe Skia benchmarking extension."
  // p_command_line->AppendSwitch(switches::kEnableSkiaBenchmarking);

  AppendUserArgs(*p_command_line);

  return content::MainFunctionParams(*p_command_line);
}

void CommandLineEfl::AppendProcessSpecificArgs(CommandLine& command_line) {
  std::string process_type = command_line.GetSwitchValueASCII(switches::kProcessType);

  if (process_type == switches::kRendererProcess) {
    command_line.AppendSwitch(switches::kDisablePlugins);
  }
  AppendUserArgs(command_line);
}

void CommandLineEfl::AppendUserArgs(CommandLine& command_line) {
  for (ArgumentVector::const_iterator it = original_arguments_.begin();
       it != original_arguments_.end(); ++it) {
    command_line.AppendSwitch(*it);
  }
}
