// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GL_GL_SHARED_CONTEXT_EFL_H
#define GL_GL_SHARED_CONTEXT_EFL_H

#include <Evas.h>

typedef struct _Evas_GL_Context Evas_GL_Context;

namespace gfx {
  class GLContext;
  class GLShareGroup;
}
namespace gpu {
namespace gles2 {
  class MailboxManager;
}
}

struct GLSharedContextEfl {
  static void Initialize(Evas_Object* object);
  static gfx::GLContext* GetInstance();
  static Evas_GL_Context* GetEvasGLContext();
  static gfx::GLShareGroup* GetShareGroup();
  static gpu::gles2::MailboxManager* GetMailboxManager();
};

#endif
