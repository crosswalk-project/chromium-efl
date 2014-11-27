// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gl/gl_image_efl_pixmap.h"

#include "base/debug/trace_event.h"
#include "base/process/process_handle.h"
#include "ui/gl/efl_pixmap.h"
#include "ui/gl/egl_util.h"

namespace {

PFNEGLCREATEIMAGEKHRPROC eglCreateImageKHRPfn = 0;
PFNGLEGLIMAGETARGETTEXTURE2DOESPROC glEGLImageTargetTexture2DOESPfn = 0;
PFNEGLDESTROYIMAGEKHRPROC eglDestroyImageKHRPfn = 0;

EGLDisplay g_display = NULL;

void EnsureGLImageFunctions() {
  if (!g_display) {
    g_display = eglGetCurrentDisplay();
    eglCreateImageKHRPfn =
        (PFNEGLCREATEIMAGEKHRPROC)eglGetProcAddress("eglCreateImageKHR");
    glEGLImageTargetTexture2DOESPfn =
        (PFNGLEGLIMAGETARGETTEXTURE2DOESPROC)eglGetProcAddress(
            "glEGLImageTargetTexture2DOES");
    eglDestroyImageKHRPfn =
        (PFNEGLDESTROYIMAGEKHRPROC)eglGetProcAddress("eglDestroyImageKHR");
  }
}

}  // namespace

namespace gfx {

GLImageEflPixmap::GLImageEflPixmap(gfx::Size size)
    : size_(size),
      in_use_(false),
      egl_image_(EGL_NO_IMAGE_KHR),
      target_(0) {
}

GLImageEflPixmap::~GLImageEflPixmap() {
}

bool GLImageEflPixmap::Initialize(unsigned long efl_pixmap_id) {
  EnsureGLImageFunctions();
  efl_pixmap_id_ = efl_pixmap_id;
  egl_image_ = eglCreateImageKHRPfn(g_display, EGL_NO_CONTEXT,
                                    EGL_NATIVE_PIXMAP_KHR,
                                    (EGLClientBuffer)(efl_pixmap_id_),
                                    NULL);

  if (egl_image_ == EGL_NO_IMAGE_KHR)
    LOG(ERROR) << "eglCreateImageKHR failed - " << ui::GetLastEGLErrorString();

  return true;
}

void GLImageEflPixmap::Destroy(bool have_context) {
  if (egl_image_ == EGL_NO_IMAGE_KHR)
    return;

  EGLBoolean success = eglDestroyImageKHRPfn(g_display, egl_image_);

  if (success == EGL_FALSE)
    LOG(ERROR) << "eglDestroyImageKHR failed - " << ui::GetLastEGLErrorString();

  efl_pixmap_id_ = 0;
  egl_image_ = EGL_NO_IMAGE_KHR;
}

gfx::Size GLImageEflPixmap::GetSize() {
  return size_;
}

bool GLImageEflPixmap::BindTexImage(unsigned target) {
  TRACE_EVENT0("gpu", "GLImageEflPixmap::BindTexImage");

  if (egl_image_ == EGL_NO_IMAGE_KHR) {
    LOG(ERROR) << "NULL EGLImage - " << ui::GetLastEGLErrorString();
    return false;
  }

  if (target_ && target_ != target) {
    LOG(ERROR) << "EGLImage can only be bound to one target";
    return false;
  }
  target_ = target;

  if (in_use_)
    return true;

  glEGLImageTargetTexture2DOESPfn(target_, (EGLClientBuffer)egl_image_);
  EGLint error = eglGetError();
  if (error != EGL_SUCCESS) {
    LOG(ERROR) << "glEGLImageTargetTexture2DOES failed - "
               << ui::GetLastEGLErrorString();
  }

  DCHECK_EQ(static_cast<GLenum>(GL_NO_ERROR), glGetError());
  in_use_ = true;
  return true;
}

void GLImageEflPixmap::ReleaseTexImage(unsigned target) {
  in_use_ = false;
}

bool GLImageEflPixmap::CopyTexImage(unsigned target) {
  return false;
}

bool GLImageEflPixmap::ScheduleOverlayPlane(gfx::AcceleratedWidget widget,
                                            int z_order,
                                            OverlayTransform transform,
                                            const Rect& bounds_rect,
                                            const RectF& crop_rect) {
  return false;
}

}  // namespace gfx
