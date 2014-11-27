// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_COMMON_GPU_CLIENT_GPU_MEMORY_BUFFER_IMPL_EFL_PIXMAP_H_
#define CONTENT_COMMON_GPU_CLIENT_GPU_MEMORY_BUFFER_IMPL_EFL_PIXMAP_H_

#include "content/common/gpu/client/gpu_memory_buffer_impl.h"

namespace gfx {
class EflPixmapImpl;
}

namespace content {

// Implementation of GPU memory buffer based on efl pixmap.
class GpuMemoryBufferImplEflPixmap : public GpuMemoryBufferImpl {
 public:
  static void Create(gfx::GpuMemoryBufferId id,
                     const gfx::Size& size,
                     Format format,
                     int client_id,
                     const CreationCallback& callback);

  static void AllocateForChildProcess(gfx::GpuMemoryBufferId id,
                                      const gfx::Size& size,
                                      Format format,
                                      int child_client_id,
                                      const AllocationCallback& callback);

  static scoped_ptr<GpuMemoryBufferImpl> CreateFromHandle(
  const gfx::GpuMemoryBufferHandle& handle,
      const gfx::Size& size,
      Format format,
      const DestructionCallback& callback);

  static void DeletedByChildProcess(gfx::GpuMemoryBufferId id,
                                    int child_client_id,
                                    uint32_t sync_point);

  static bool IsFormatSupported(Format format);
  static bool IsUsageSupported(Usage usage);
  static bool IsLayoutSupported(const gfx::Size& size, Format format);
  static bool IsConfigurationSupported(Format format, Usage usage);

  // Overridden from gfx::GpuMemoryBuffer:
  virtual void* Map() override;
  virtual void Unmap() override;
  virtual uint32 GetStride() const override;
  virtual gfx::GpuMemoryBufferHandle GetHandle() const override;

 private:
  GpuMemoryBufferImplEflPixmap(gfx::GpuMemoryBufferId id,
                               const gfx::Size& size,
                               Format format,
                               const DestructionCallback& callback,
                               scoped_ptr<gfx::EflPixmapImpl> efl_pixmap);
  virtual ~GpuMemoryBufferImplEflPixmap();

  gfx::GpuMemoryBufferId id_;
  scoped_ptr<gfx::EflPixmapImpl> efl_pixmap_;
  size_t stride_;

  DISALLOW_COPY_AND_ASSIGN(GpuMemoryBufferImplEflPixmap);
};

}  // namespace content

#endif  // CONTENT_COMMON_GPU_CLIENT_GPU_MEMORY_BUFFER_IMPL_EFL_PIXMAP_H_
