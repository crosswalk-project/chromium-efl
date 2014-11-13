// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/public/app/content_main.h"

#include <Ecore_Evas.h>
#include <Ecore_X.h>

#include "content/shell/app/shell_main_delegate.h"
#include "content/browser/gpu/gpu_process_host.h"
#include "base/message_loop/message_loop.h"
#include "gpu/gpu_thread_override_efl.h"
#include "shell/message_pump_efl.h"
#include "screen_efl.h"

scoped_ptr<base::MessagePump> MessagePumpFactory() {
  return scoped_ptr<base::MessagePump>(new base::MessagePumpEfl);
}

int main(int argc, const char* argv[]) {
  ecore_init();
  ecore_x_init(NULL);
  eina_init();
  evas_init();
  if (!base::MessageLoop::InitMessagePumpForUIFactory(&MessagePumpFactory)) {
    LOG(ERROR) << "Failed to install EFL message pump!";
    return 1;
  }

  ui::InstallScreenInstance();
  content::GpuProcessHost::RegisterGpuMainThreadFactory(CreateInProcessGpuThreadEfl);

  content::ShellMainDelegate delegate;

  content::ContentMainParams prams(&delegate);
  prams.argc = argc;
  prams.argv = argv;
  return content::ContentMain(prams);
}
