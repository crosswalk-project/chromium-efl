// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifdef TIZEN_AUTOFILL_SUPPORT

#include "browser/password_manager/password_manager_util.h"

namespace password_manager_util {

bool AuthenticateUser(gfx::NativeWindow window) {
  return true;
}

OsPasswordStatus GetOsPasswordStatus()
{
  return PASSWORD_STATUS_UNSUPPORTED;
}

}  // namespace password_manager_util

#endif // TIZEN_AUTOFILL_SUPPORT
