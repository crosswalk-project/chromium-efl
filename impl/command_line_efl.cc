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
  p_command_line->AppendSwitch(switches::kEnablePinch);
  p_command_line->AppendSwitchASCII(switches::kUseGL, gfx::kGLImplementationEGLName);
#if !defined(EWK_BRINGUP)
  p_command_line->AppendSwitch(switches::kEnableGestureTapHighlight);
#endif
  p_command_line->AppendSwitch(switches::kEnableSpatialNavigation);
  p_command_line->AppendSwitch(switches::kMainFrameResizesAreOrientationChanges);

  // FIXME(Kapil) Will be removed after permission handling implementation.
  p_command_line->AppendSwitch(switches::kDisableWebSecurity);
#else
  p_command_line->AppendSwitchASCII(switches::kUseGL, gfx::kGLImplementationDesktopName);
  p_command_line->AppendSwitch(switches::kDisableDelegatedRenderer);
#endif

  p_command_line->AppendSwitch(switches::kDisableDelegatedRenderer);

  //if we use software path we dont need to have next switches
#if defined(OS_TIZEN)
#if !defined(EWK_BRINGUP)
  if (!p_command_line->HasSwitch(switches::kUseSWRenderingPath))
#endif
#endif
  {
#warning "[M37] Investigae removed command line switches, are they still needed, do they have a replacement?"
    //p_command_line->AppendSwitch(switches::kForceCompositingMode);
  // [M37] Note: The commit "Temporarily disable zero copy as it causes browser crash during regression"
  // is to deprecate kEnableMapImage option.
  // But it was already deprecated during fixing M37 build as no command line option with such name (see above comment)
  // TODO: remove this commit if it turn out the option is unnecessary
  //Disabling temporarily, as it causes browser crash ID:335 in regression
    //p_command_line->AppendSwitch(cc::switches::kEnableMapImage);
    p_command_line->AppendSwitch(switches::kEnableThreadedCompositing);
    p_command_line->AppendSwitch(switches::kIgnoreGpuBlacklist);
  }

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
