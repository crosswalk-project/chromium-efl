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

#ifndef GL_GL_SHARED_CONTEXT_EFL_H
#define GL_GL_SHARED_CONTEXT_EFL_H

typedef struct _Evas_Object Evas_Object;
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
