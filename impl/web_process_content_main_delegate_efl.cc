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
