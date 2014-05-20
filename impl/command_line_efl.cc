#include "command_line_efl.h"

#include <string>
#include <cstring>
#include <sys/types.h>
#include <unistd.h>

#include "content/public/common/content_switches.h"
#include "content/public/common/content_client.h"
#include "ui/events/event_switches.h"
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

  // XXX: kInProcessGPU in not exported by content API
  // (see: content/public/common/content_switches.h)
  p_command_line->AppendSwitch("--in-process-gpu");

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
#endif

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
