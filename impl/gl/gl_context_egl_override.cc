// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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

