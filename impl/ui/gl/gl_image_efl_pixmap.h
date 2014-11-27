// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GL_GL_IMAGE_EFL_PIXMAP_H_
#define UI_GL_GL_IMAGE_EFL_PIXMAP_H_

#include "base/memory/scoped_ptr.h"
#include "ui/gl/gl_image.h"
#include "ui/gfx/gpu_memory_buffer.h"
#include "ui/gl/gl_bindings.h"


namespace gfx {
class EflPixmap;

class GL_EXPORT GLImageEflPixmap : public GLImage {
 public:
  GLImageEflPixmap(gfx::Size size);

  bool Initialize(unsigned long efl_pixmap_id);

  // Overridden from GLImage:
  virtual void Destroy(bool have_context) override;
  virtual gfx::Size GetSize() override;
  virtual bool BindTexImage(unsigned target) override;
  virtual void ReleaseTexImage(unsigned target) override;
  virtual bool CopyTexImage(unsigned target) override;
  virtual void WillUseTexImage() override {}
  virtual void DidUseTexImage() override {}
  virtual void WillModifyTexImage() override {}
  virtual void DidModifyTexImage() override {}
  virtual bool ScheduleOverlayPlane(gfx::AcceleratedWidget widget,
                                    int z_order,
                                    OverlayTransform transform,
                                    const Rect& bounds_rect,
                                    const RectF& crop_rect) override;

 protected:
  virtual ~GLImageEflPixmap();

 private:
  unsigned long efl_pixmap_id_;
  const gfx::Size size_;
  bool in_use_;
  EGLImageKHR egl_image_;
  unsigned target_;

  DISALLOW_COPY_AND_ASSIGN(GLImageEflPixmap);
};

}  // namespace gfx

#endif  // UI_GL_GL_IMAGE_EFL_PIXMAP_H_
