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

void CommandLineEfl::Init(int argc, char *argv[]) {
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
}

content::MainFunctionParams CommandLineEfl::GetDefaultPortParams() {
  CommandLine* p_command_line = CommandLine::ForCurrentProcess();

  p_command_line->AppendSwitch(switches::kNoSandbox);
  p_command_line->AppendSwitch(switches::kDisablePlugins);
  p_command_line->AppendSwitch(switches::kInProcessGPU);

  p_command_line->AppendSwitch(switches::kUseMobileUserAgent);
  p_command_line->AppendSwitch(switches::kEnableViewportMeta);
#if defined(OS_TIZEN)
  p_command_line->AppendSwitch(switches::kEnableOverscrollNotifications);
  p_command_line->AppendSwitch(switches::kTouchEvents);
  p_command_line->AppendSwitchASCII(switches::kUseGL, gfx::kGLImplementationEGLName);
  p_command_line->AppendSwitch(switches::kEnableGestureTapHighlight);
  p_command_line->AppendSwitch(switches::kEnableSpatialNavigation);
#else
  p_command_line->AppendSwitchASCII(switches::kUseGL, gfx::kGLImplementationDesktopName);
  p_command_line->AppendSwitch(switches::kDisableDelegatedRenderer);
#endif

  p_command_line->AppendSwitch(switches::kDisableDelegatedRenderer);
  p_command_line->AppendSwitch(switches::kDisableImplSidePainting);

  p_command_line->AppendSwitch(switches::kForceCompositingMode);
  p_command_line->AppendSwitch(switches::kEnableThreadedCompositing);
  p_command_line->AppendSwitch(switches::kIgnoreGpuBlacklist);
  p_command_line->AppendSwitch(switches::kAllowWebUICompositing);
  p_command_line->AppendSwitch(switches::kEnablePinch);

  // FIXME: Crash is observed if this switch below is applied for TV. So for now enabling
  //        it only for mobile. More specifically, below line causes rendering issues in
  //        chromium upversion (M34_1809-> M34_1847).
#if defined(OS_TIZEN_MOBILE)
  p_command_line->AppendSwitch(cc::switches::kEnableMapImage);
#endif
  p_command_line->AppendSwitch(cc::switches::kEnableImplSidePainting);

  // XXX: Skia benchmarking should be only used for testing,
  // when enabled the following warning is printed to stderr:
  // "Enabling unsafe Skia benchmarking extension."
  // p_command_line->AppendSwitch(switches::kEnableSkiaBenchmarking);

  p_command_line->AppendSwitch("--enable-pinch");

  AppendUserArgs(*p_command_line);

  return content::MainFunctionParams(*p_command_line);
}

void CommandLineEfl::AppendProcessSpecificArgs(CommandLine& command_line) {
  std::string process_type = command_line.GetSwitchValueASCII(switches::kProcessType);

  if (process_type == switches::kRendererProcess) {
    command_line.AppendSwitchNative(switches::kUserAgent, content::GetUserAgent(GURL()));
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
