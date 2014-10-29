// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/logging.h"
#include "web_process_content_main_delegate_efl.h"

int main(int argc, const char **argv) {
  LOG(INFO) << "web process launching...";

  content::WebProcessContentMainDelegateEfl content_main_delegate;
  content::ContentMainParams params(&content_main_delegate);

  params.argc = argc;
  params.argv = argv;

  return content::ContentMain(params);
}
