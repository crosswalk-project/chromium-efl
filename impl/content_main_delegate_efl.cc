// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content_main_delegate_efl.h"

#include "base/path_service.h"
#include "content/browser/gpu/gpu_process_host.h"
#include "content/gpu/in_process_gpu_thread.h"
#include "content/public/common/content_switches.h"
#include "browser/resource_dispatcher_host_delegate_efl.h"
#include "command_line_efl.h"
#include "content_browser_client_efl.h"
#include "gpu/gpu_thread_override_efl.h"
#include "paths_efl.h"

namespace content {

std::string SubProcessPath() {
  static std::string result;
  if (!result.empty())
    return result;

  char* path_from_env = getenv("EFL_WEBPROCESS_PATH");
  if (path_from_env) {
    result = std::string(path_from_env);
    return result;
  }

  base::FilePath pak_dir;
  base::FilePath pak_file;
  PathService::Get(base::DIR_EXE, &pak_dir);
  pak_file = pak_dir.Append(FILE_PATH_LITERAL("efl_webprocess"));

  return pak_file.AsUTF8Unsafe();
}

void InitializeUserDataDir() {
  CommandLine* command_line = CommandLine::ForCurrentProcess();

  base::FilePath user_data_dir = command_line->GetSwitchValuePath("user-data-dir");
  if (!user_data_dir.empty() && !PathService::OverrideAndCreateIfNeeded(
      PathsEfl::DIR_USER_DATA, user_data_dir, true, true)) {
    DLOG(WARNING) << "Could not set user data directory to " << user_data_dir.value();

    if (!PathService::Get(PathsEfl::DIR_USER_DATA, &user_data_dir))
      CHECK(false) << "Could not get default user data directory";

    command_line->AppendSwitchPath("user-data-dir", user_data_dir);
  }
}

void InitializeDiskCacheDir() {
  CommandLine* command_line = CommandLine::ForCurrentProcess();

  base::FilePath disk_cache_dir = command_line->GetSwitchValuePath("disk-cache-dir");
  if (!disk_cache_dir.empty() && !PathService::OverrideAndCreateIfNeeded(
      base::DIR_CACHE, disk_cache_dir, true, true)) {
    DLOG(WARNING) << "Could not set disk cache directory to " << disk_cache_dir.value();

    if (!PathService::Get(base::DIR_CACHE, &disk_cache_dir))
      CHECK(false) << "Could not get default disk cache directory";

    command_line->AppendSwitchPath("disk-cache-dir", disk_cache_dir);
  }
}

void ContentMainDelegateEfl::PreSandboxStartup() {
  PathService::Override(base::FILE_EXE, base::FilePath(SubProcessPath()));

  InitializeUserDataDir();
  InitializeDiskCacheDir();

  // needed for gpu process
  CommandLine::ForCurrentProcess()->AppendSwitchPath(
      switches::kBrowserSubprocessPath, base::FilePath(SubProcessPath()));

  // needed for gpu thread
  GpuProcessHost::RegisterGpuMainThreadFactory(CreateInProcessGpuThreadEfl);
}

ContentBrowserClient* ContentMainDelegateEfl::CreateContentBrowserClient() {
  client_.reset(new ContentBrowserClientEfl);
  return client_.get();
}

bool ContentMainDelegateEfl::BasicStartupComplete(int* /*exit_code*/) {
  content::SetContentClient(&content_client_);
  PathsEfl::Register();
  return false;
}

} // namespace content
