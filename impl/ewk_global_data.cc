#include "ewk_global_data.h"

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
#include "content/renderer/in_process_renderer_thread.h"
#include "content/utility/in_process_utility_thread.h"
#include "ui/base/resource/resource_bundle.h"

#include "command_line_efl.h"
#include "content_main_delegate_efl.h"
#include "message_pump_for_ui_efl.h"
#include "screen_efl.h"

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

base::MessagePump* MessagePumpFactory() {
  return new base::MessagePumpForUIEfl;
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

  instance_ = new EwkGlobalData();

  bool message_pump_overridden = base::MessageLoop::InitMessagePumpForUIFactory(&MessagePumpFactory);
  DCHECK(message_pump_overridden);

  content::InstallScreenInstance();

  instance_->content_main_runner_->Initialize(CommandLineEfl::GetArgc(),
      CommandLineEfl::GetArgv(), new ContentMainDelegateEfl());
  instance_->browser_main_runner_->Initialize(CommandLineEfl::GetDefaultPortParams());

  base::ThreadRestrictions::SetIOAllowed(true);

  base::FilePath pak_dir;
  base::FilePath pak_file;
  PathService::Get(base::DIR_EXE, &pak_dir);
  pak_file = pak_dir.Append(FILE_PATH_LITERAL("content_shell.pak"));
  ui::ResourceBundle::InitSharedInstanceWithPakPath(pak_file);

  if (CommandLine::ForCurrentProcess()->HasSwitch(switches::kSingleProcess)) {
    UtilityProcessHost::RegisterUtilityMainThreadFactory(
        content::CreateInProcessUtilityThread);
    RenderProcessHost::RegisterRendererMainThreadFactory(
        content::CreateInProcessRendererThread);
    GpuProcessHost::RegisterGpuMainThreadFactory(
        content::CreateInProcessGpuThread);
  }

#ifndef NDEBUG
  logging::LoggingSettings settings;
  settings.logging_dest = logging::LOG_TO_SYSTEM_DEBUG_LOG;
  logging::InitLogging(settings);
  logging::SetLogItems(true, true, true, true);
#endif
}
