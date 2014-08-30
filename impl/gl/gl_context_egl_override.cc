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

// Define GLContextEGL to GLContextEGLOverride to be able to replace
// initialize method in GLContextEGL
#define GLContextEGL GLContextEGLOverride

#define private public

#include "ui/gl/gl_context_egl.cc"

#undef GLContextEGL

namespace gfx {

struct GL_EXPORT GLContextEGL : public GLContextEGLOverride {
  GLContextEGL(GLShareGroup* share_group);

  bool Initialize(
      GLSurface* compatible_surface, GpuPreference gpu_preference);
};

GLContextEGL::GLContextEGL(GLShareGroup* share_group) :
    GLContextEGLOverride(share_group) {
}

bool GLContextEGL::Initialize(
    GLSurface* compatible_surface, GpuPreference gpu_preference) {
  DCHECK(compatible_surface);
  DCHECK(!context_);

  static const EGLint kContextAttributes[] = {
    EGL_CONTEXT_CLIENT_VERSION, 2,
    EGL_NONE
  };

  display_ = compatible_surface->GetDisplay();
  config_ = compatible_surface->GetConfig();

  context_ = eglCreateContext(
      display_,
      config_,
      share_group() ? share_group()->GetHandle() : NULL,
      kContextAttributes);

  if (!context_) {
    LOG(ERROR) << "eglCreateContext failed with error "
               << GetLastEGLErrorString();
    return false;
  }

  return true;
}

}

