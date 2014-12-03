// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ewk_main_private.h"
#include "webview_delegate_ewk.h"

std::string g_homeDirectory;

const char* ewk_home_directory_get()
{
  if (!g_homeDirectory.empty())
    return g_homeDirectory.c_str();

  const char *home = getenv("HOME");
  if (!home)
    home = getenv("TMPDIR");
  if (!home)
    home = "/tmp";

  return home;
}
