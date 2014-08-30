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

// Define GLContextGLX to GLContextGLXOverride to be able to replace
// initialize method in GLContextGLX
#define GLContextGLX GLContextGLXOverride

#define private public

#include "ui/gl/gl_context_glx.cc"

#undef GLContextGLX

namespace gfx {

struct GL_EXPORT GLContextGLX : public GLContextGLXOverride {
  GLContextGLX(GLShareGroup* share_group);

  bool Initialize(
      GLSurface* compatible_surface, GpuPreference gpu_preference);
};

GLContextGLX::GLContextGLX(GLShareGroup* share_group) :
    GLContextGLXOverride(share_group) {
}

bool GLContextGLX::Initialize(
    GLSurface* compatible_surface, GpuPreference gpu_preference) {
  display_ = static_cast<XDisplay*>(compatible_surface->GetDisplay());

  GLXContext share_handle = static_cast<GLXContext>(
      share_group() ? share_group()->GetHandle() : NULL);

  context_ = glXCreateNewContext(
     display_,
     static_cast<GLXFBConfig>(compatible_surface->GetConfig()),
     GLX_RGBA_TYPE,
     share_handle,
     True);
  if (!context_) {
    LOG(ERROR) << "Failed to create GL context with glXCreateNewContext.";
    return false;
  }

  DCHECK(context_);
  DVLOG(1) << "  Successfully allocated "
           << (compatible_surface->IsOffscreen() ?
               "offscreen" : "onscreen")
           << " GL context with LOSE_CONTEXT_ON_RESET_ARB";

  DVLOG(1) << (compatible_surface->IsOffscreen() ? "Offscreen" : "Onscreen")
           << " context was "
           << (glXIsDirect(display_,
                           static_cast<GLXContext>(context_))
                   ? "direct" : "indirect")
           << ".";

  return true;
}

}

