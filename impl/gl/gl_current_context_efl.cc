// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gl/gl_implementation.h"
#include "ui/gl/gl_implementation_osmesa.h"
#include "gpu/command_buffer/service/texture_manager.h"

using namespace gfx;


extern void* GLGetCurentContext() {
  // Temprorarily load corresponding gl library and shutdown for
  // later correct initialization.
  // Chromium gl system is not initialized yet and evas gl doesn't
  // expose "GetCurrentContext" or "GetNativeContextHandle" API.
  base::NativeLibrary library =
      LoadLibraryAndPrintError("libEGL.so.1");
  typedef EGLContext (*eglGetCurrentContextProc)(void);

  eglGetCurrentContextProc proc = reinterpret_cast<eglGetCurrentContextProc>(
      base::GetFunctionPointerFromNativeLibrary(
          library, "eglGetCurrentContext"));

  void* handle = proc();
  base::UnloadNativeLibrary(library);
  return handle;
}

namespace content {
GLuint GetTextureIdFromTexture(gpu::gles2::Texture* texture) {
  return texture->service_id();
}
}
