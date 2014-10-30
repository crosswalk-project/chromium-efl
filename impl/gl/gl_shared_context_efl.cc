// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gl/gl_shared_context_efl.h"

#include "gpu/command_buffer/service/mailbox_manager_impl.h"
#include "ui/gl/gl_context.h"
#include "ui/gl/gl_share_group.h"

#include <Ecore_Evas.h>
#include <Evas_GL.h>

// Defined in gl_current_context_efl.cc because of conflict in chromium
// and efl gl includes.
extern void* GLGetCurentContext();

struct GLSharedContextEflPrivate : public gfx::GLContext {
  virtual bool Initialize(
      gfx::GLSurface*, gfx::GpuPreference) override {
    NOTREACHED();
  }

  virtual void Destroy() override {
    NOTREACHED();
  }

  virtual bool MakeCurrent(gfx::GLSurface*) override {
    NOTREACHED();
    return false;
  }

  virtual void ReleaseCurrent(gfx::GLSurface*) override {
    NOTREACHED();
  }

  virtual bool IsCurrent(gfx::GLSurface*) override {
    NOTREACHED();
    return false;
  }

  virtual void* GetHandle() override {
    return handle_;
  }

  virtual void SetSwapInterval(int) override {
    NOTREACHED();
  }

  GLSharedContextEflPrivate(Evas_Object* object);

  ~GLSharedContextEflPrivate();

  static scoped_refptr<GLSharedContextEflPrivate>& instance() {
    static scoped_refptr<GLSharedContextEflPrivate> i;
    return i;
  }


  void* handle_;
  Evas_GL* evas_gl_;
  Evas_GL_Context* evas_gl_context_;
  Evas_GL_Surface* evas_gl_surface_;
  Evas_GL_Config* evas_gl_config_;
};

GLSharedContextEflPrivate::GLSharedContextEflPrivate(Evas_Object* object) :
    GLContext(GLSharedContextEfl::GetShareGroup()) {
  Evas* evas =  evas_object_evas_get(object);
  evas_gl_config_ = evas_gl_config_new();
  evas_gl_config_->options_bits = EVAS_GL_OPTIONS_NONE;
  evas_gl_config_->color_format = EVAS_GL_RGBA_8888;
  evas_gl_config_->depth_bits = EVAS_GL_DEPTH_BIT_24;
  evas_gl_config_->stencil_bits = EVAS_GL_STENCIL_BIT_8;

  evas_gl_ = evas_gl_new(evas);
  evas_gl_context_ = evas_gl_context_create(evas_gl_, 0);
  if (!evas_gl_context_) {
    LOG(ERROR) << "GLSharedContextEflPrivate(): Create evas gl context Fail";
  }

  evas_gl_surface_ = evas_gl_surface_create(
      evas_gl_, evas_gl_config_, 1, 1);
  if (!evas_gl_surface_) {
    LOG(ERROR) << "GLSharedContextEflPrivate(): Create evas gl Surface Fail";
  }

  evas_gl_make_current(evas_gl_, evas_gl_surface_, evas_gl_context_);
  handle_ = GLGetCurentContext();
  evas_gl_make_current(evas_gl_, 0, 0);
}

GLSharedContextEflPrivate::~GLSharedContextEflPrivate() {
  evas_gl_surface_destroy(evas_gl_, evas_gl_surface_);
  evas_gl_context_destroy(evas_gl_, evas_gl_context_);
  evas_gl_config_free(evas_gl_config_);
  evas_gl_free(evas_gl_);
}

// static
void GLSharedContextEfl::Initialize(Evas_Object* object) {
  if (GLSharedContextEflPrivate::instance().get())
    return;

  GLSharedContextEflPrivate::instance() =
      new GLSharedContextEflPrivate(object);
}

// static
gfx::GLContext* GLSharedContextEfl::GetInstance() {
  return GLSharedContextEflPrivate::instance().get();
}

// static
Evas_GL_Context* GLSharedContextEfl::GetEvasGLContext() {
  return GLSharedContextEflPrivate::instance()->evas_gl_context_;
}

// static
gfx::GLShareGroup* GLSharedContextEfl::GetShareGroup() {
  static scoped_refptr<gfx::GLShareGroup> share_group_ =
      new gfx::GLShareGroup();
  return share_group_.get();
}

// static
gpu::gles2::MailboxManager* GLSharedContextEfl::GetMailboxManager() {
  static scoped_refptr<gpu::gles2::MailboxManager> mailbox_manager_ =
      new gpu::gles2::MailboxManagerImpl();
  return mailbox_manager_.get();
}

