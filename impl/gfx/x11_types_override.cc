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

// Define OpenNewXDisplay and GetXDisplay to some unused name to be able to
// replace it with our own implementation.
#define OpenNewXDisplay OpenNewXDisplayOverride
#define GetXDisplay  GetXDisplayOverride

#include "ui/gfx/x/x11_types.cc"

#undef GetXDisplay
#undef OpenNewXDisplay

#include <Ecore_X.h>
#include <X11/Xlib.h>

namespace gfx {
Display* OpenNewXDisplay() {
  static Display* display = static_cast<Display*>(ecore_x_display_get());
  return display;
}

XDisplay* GetXDisplay() {
  return OpenNewXDisplay();
}

}
