// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/events/platform/x11/x11_event_source.h"

#include <X11/Xlib.h>

namespace ui {

// Dummy Efl X11 event source to replace glib/libevent sources that were
// stealing X11 events from efl.
class X11EventSourceEfl : public X11EventSource {
 public:
  explicit X11EventSourceEfl(XDisplay* display)
      : X11EventSource(display) {
  }

};
scoped_ptr<PlatformEventSource> PlatformEventSource::CreateDefault() {
  return scoped_ptr<PlatformEventSource>(
      new X11EventSourceEfl(gfx::GetXDisplay()));
}

} // namespace ui
