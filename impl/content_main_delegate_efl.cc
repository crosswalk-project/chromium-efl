/*
   Copyright (C) 2014 Samsung Electronics

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "content_main_delegate_efl.h"

#include "base/path_service.h"
#include "content/browser/gpu/gpu_process_host.h"
#include "content/gpu/in_process_gpu_thread.h"
#include "content/public/common/content_switches.h"
#include "browser/resource_dispatcher_host_delegate_efl.h"
#include "command_line_efl.h"
#include "content_browser_client_efl.h"
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

void ContentMainDelegateEfl::PreSandboxStartup() {
  PathService::Override(base::FILE_EXE, base::FilePath(SubProcessPath()));

  // needed for gpu process
  CommandLine::ForCurrentProcess()->AppendSwitchPath(
      switches::kBrowserSubprocessPath, base::FilePath(SubProcessPath()));

  // needed for gpu thread
  GpuProcessHost::RegisterGpuMainThreadFactory(CreateInProcessGpuThread);
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
