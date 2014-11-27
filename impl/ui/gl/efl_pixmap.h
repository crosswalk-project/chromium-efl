// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GL_EFL_PIXMAP_H_
#define UI_GL_EFL_PIXMAP_H_

#include "base/containers/hash_tables.h"
#include "base/containers/scoped_ptr_hash_map.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "base/memory/scoped_vector.h"
#include "cc/base/ref_counted_managed.h"
#include "ui/gfx/size.h"
#include "ui/gl/gl_bindings.h"


namespace gfx {

class NativeBuffer;

// TODO(prashant.n): Refactor the code into different files.
// Basic Pixmap implementation.
class GL_EXPORT EflPixmapBase {
 public:
  enum UsageType {
    BUFFER,
    SURFACE
  };

  virtual unsigned long GetId() { return pixmap_id_; }

  virtual void* Map();
  virtual bool UnMap();
  virtual int GetStride();

 protected:
  EflPixmapBase(UsageType usage_type);
  virtual ~EflPixmapBase();

  UsageType usage_type_;
  unsigned long pixmap_id_;
  NativeBuffer* native_buffer_;

  static int s_pixmap_surface_count;
};

// Pixmap implementation - to be used in GPU process/thread.
class GL_EXPORT EflPixmap : public EflPixmapBase,
                            public base::RefCountedThreadSafe<EflPixmap> {
 public:
  static scoped_refptr<EflPixmap> Create(UsageType usage_type,
                                         const gfx::Size& size);
  virtual ~EflPixmap();

  static const char* GetEglExtensions();
  static Display* GetNativeDisplay();
  static EGLDisplay GetDisplay();
  static EGLConfig GetConfig();

  const gfx::Size GetSize() const { return size_; }
  bool Used() { return used_; }
  void SetUsed(bool used) { used_ = used; }
  EGLSurface GetSurface() { return surface_; }

  // EflPixmapBase functions:
  using EflPixmapBase::GetId;
  using EflPixmapBase::Map;
  using EflPixmapBase::UnMap;
  using EflPixmapBase::GetStride;

 private:
  EflPixmap(UsageType usage_type, const gfx::Size& size);
  bool Initialize();
  static EGLConfig ChooseConfig();

  EGLSurface CreateSurface(unsigned long pixmap_id);
  void DestroySurface(EGLSurface surface);
  unsigned long CreatePixmap(const gfx::Size& size);
  void FreePixmap(unsigned long pixmap_id);

  gfx::Size size_;
  EGLSurface surface_;
  bool used_;

  DISALLOW_COPY_AND_ASSIGN(EflPixmap);
};

// Pixmap implementation - to be used in renderer and browser processes.
class GL_EXPORT EflPixmapImpl : public EflPixmapBase {
 public:
  explicit EflPixmapImpl(UsageType usage_type, unsigned long pixmap_id);
  ~EflPixmapImpl();

  // EflPixmapBase functions:
  using EflPixmapBase::GetId;
  using EflPixmapBase::Map;
  using EflPixmapBase::UnMap;
  using EflPixmapBase::GetStride;

 private:
  DISALLOW_COPY_AND_ASSIGN(EflPixmapImpl);
};

}  // namespace gfx

#endif  // UI_GL_EFL_PIXMAP_H_
