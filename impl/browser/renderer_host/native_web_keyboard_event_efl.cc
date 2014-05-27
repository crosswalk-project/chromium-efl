// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/public/browser/native_web_keyboard_event.h"

namespace content {

NativeWebKeyboardEvent::NativeWebKeyboardEvent()
    : os_event(NULL),
      skip_in_browser(false) {
}

NativeWebKeyboardEvent::NativeWebKeyboardEvent(const NativeWebKeyboardEvent&)
{
}

NativeWebKeyboardEvent::~NativeWebKeyboardEvent()
{
}

NativeWebKeyboardEvent& NativeWebKeyboardEvent::operator=(const NativeWebKeyboardEvent&)
{
  return *this;
}

}
