// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "web_process_content_main_delegate_efl.h"

#include "base/files/file_path.h"
#include "base/path_service.h"
#include "content/public/common/referrer.h"
#include "ui/base/resource/resource_bundle.h"
#include "paths_efl.h"
#include "renderer/content_renderer_client_efl.h"
#include "common/content_client_efl.h"

namespace content {

// static
ContentRendererClient* WebProcessContentMainDelegateEfl::CreateContentRendererClient() {
  return new ContentRendererClientEfl();
}

// ContentMainDelegate implementation:
void WebProcessContentMainDelegateEfl::PreSandboxStartup() {
  base::FilePath pak_dir;
  base::FilePath pak_file;
  PathService::Get(base::DIR_EXE, &pak_dir);
  pak_file = pak_dir.Append(FILE_PATH_LITERAL("content_shell.pak"));
  ui::ResourceBundle::InitSharedInstanceWithPakPath(pak_file);
}

bool WebProcessContentMainDelegateEfl::BasicStartupComplete(int* exit_code) {
  PathsEfl::Register();
  ContentClientEfl* content_client = new ContentClientEfl();
  content::SetContentClient(content_client);
  return false;
}

} // namespace content
