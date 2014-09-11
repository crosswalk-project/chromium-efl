/*
 * Copyright (C) 2014 Samsung Electronics. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 */


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
