#include "ewk_global_data.h"

#include "base/cpu.h"
#include "base/logging.h"
#include "base/path_service.h"
#include "base/message_loop/message_loop.h"
#include "content/browser/gpu/gpu_process_host.h"
#include "content/gpu/in_process_gpu_thread.h"
#include "content/public/app/content_main_runner.h"
#include "content/public/browser/browser_main_runner.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/browser/utility_process_host.h"
#include "content/public/common/content_switches.h"
#include "content/public/app/content_main.h"
#include "content/renderer/in_process_renderer_thread.h"
#include "content/utility/in_process_utility_thread.h"
#include "content/browser/utility_process_host_impl.h"
#include "content/browser/renderer_host/render_process_host_impl.h"
#include "gpu/gpu_thread_override_efl.h"
#include "ui/base/resource/resource_bundle.h"

#include "command_line_efl.h"
#include "content_main_delegate_efl.h"
#include "message_pump_for_ui_efl.h"
#include "screen_efl.h"

#ifdef OS_TIZEN_MOBILE
#include <dlfcn.h>
void* EflAssistHandle = 0;
#endif

using base::MessageLoop;
using content::BrowserMainRunner;
using content::BrowserThread;
using content::ContentMainDelegateEfl;
using content::ContentMainRunner;
using content::GpuProcessHost;
using content::RenderProcessHost;
using content::UtilityProcessHost;

EwkGlobalData* EwkGlobalData::instance_ = NULL;

namespace {

scoped_ptr<base::MessagePump> MessagePumpFactory() {
  return scoped_ptr<base::MessagePump>(new base::MessagePumpForUIEfl);
}

} // namespace

EwkGlobalData::EwkGlobalData()
  : content_main_runner_(ContentMainRunner::Create())
  , browser_main_runner_(BrowserMainRunner::Create()) {
}

EwkGlobalData::~EwkGlobalData() {
    DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
    // We need to pretend that message loop was stopped so chromium unwinds correctly
    MessageLoop *loop = MessageLoop::current();
    loop->QuitNow();
    // browser_main_runner must be deleted first as it depends on content_main_runner
    delete browser_main_runner_;
    delete content_main_runner_;
}

EwkGlobalData* EwkGlobalData::GetInstance() {
  return instance_;
}

void EwkGlobalData::Ensure() {
  if (instance_)
    return;

  // Workaround for cpu info logging asserting if executed on the wrong thread
  // during cpu info lazy instance initialization.
  base::CPU cpu;
  DCHECK(cpu.cpu_brand() != "");

  instance_ = new EwkGlobalData();

  bool message_pump_overridden =
      base::MessageLoop::InitMessagePumpForUIFactory(&MessagePumpFactory);
  DCHECK(message_pump_overridden);

  content::InstallScreenInstance();

	content::ContentMainParams params(new ContentMainDelegateEfl());
	params.argc = CommandLineEfl::GetArgc();
	params.argv = CommandLineEfl::GetArgv();

  // Call to CommandLineEfl::GetDefaultPortParams() should be before content
  // main runner initialization in order to pass command line parameters
  // for current process that are used in content main runner initialization.
  content::MainFunctionParams main_funtion_params =
    CommandLineEfl::GetDefaultPortParams();

  instance_->content_main_runner_->Initialize(params);
  instance_->browser_main_runner_->Initialize(main_funtion_params);

  base::ThreadRestrictions::SetIOAllowed(true);

  base::FilePath pak_dir;
  base::FilePath pak_file;
  PathService::Get(base::DIR_EXE, &pak_dir);
  pak_file = pak_dir.Append(FILE_PATH_LITERAL("content_shell.pak"));
  ui::ResourceBundle::InitSharedInstanceWithPakPath(pak_file);

  if (CommandLine::ForCurrentProcess()->HasSwitch(switches::kSingleProcess)) {
    content::UtilityProcessHostImpl::RegisterUtilityMainThreadFactory(
        content::CreateInProcessUtilityThread);
    content::RenderProcessHostImpl::RegisterRendererMainThreadFactory(
        content::CreateInProcessRendererThread);
    GpuProcessHost::RegisterGpuMainThreadFactory(
        CreateInProcessGpuThreadEfl);
  }

#ifndef NDEBUG
  logging::LoggingSettings settings;
  settings.logging_dest = logging::LOG_TO_SYSTEM_DEBUG_LOG;
  logging::InitLogging(settings);
  logging::SetLogItems(true, true, true, true);
#endif

#ifdef OS_TIZEN_MOBILE
  if (!EflAssistHandle)
    EflAssistHandle = dlopen("/usr/lib/libefl-assist.so.0", RTLD_LAZY);
#endif
}
