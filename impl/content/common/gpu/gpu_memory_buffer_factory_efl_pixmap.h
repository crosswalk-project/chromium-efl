// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_COMMON_GPU_GPU_MEMORY_BUFFER_FACTORY_EFL_PIXMAP_H_
#define CONTENT_COMMON_GPU_GPU_MEMORY_BUFFER_FACTORY_EFL_PIXMAP_H_

#include "base/containers/hash_tables.h"
#include "base/memory/ref_counted.h"
#include "ui/gfx/geometry/size.h"
#include "ui/gfx/gpu_memory_buffer.h"

namespace gfx {
class GLImage;
class EflPixmap;
}

namespace content {

class GpuMemoryBufferFactoryEflPixmap {
 public:
  GpuMemoryBufferFactoryEflPixmap();
  ~GpuMemoryBufferFactoryEflPixmap();

  // Creates a efl pixmap backed GPU memory buffer with |size| and
  // |internalformat|. A valid handle is returned on success.
  gfx::GpuMemoryBufferHandle CreateGpuMemoryBuffer(
      gfx::GpuMemoryBufferId id,
      const gfx::Size& size,
      unsigned internalformat,
      int client_id);

  // Destroy a previously created GPU memory buffer.
  void DestroyGpuMemoryBuffer(const gfx::GpuMemoryBufferId id,int client_id);

  // Creates a GLImage instance for a GPU memory buffer.
  scoped_refptr<gfx::GLImage> CreateImageForGpuMemoryBuffer(
      unsigned long frame_pixmap_id,
      const gfx::Size& size,
      unsigned internalformat,
      int client_id);

 private:
  typedef std::pair<int, int> EflPixmapMapKey;
  typedef base::hash_map<EflPixmapMapKey,
                         scoped_refptr<gfx::EflPixmap>> EflPixmapMap;
  EflPixmapMap efl_pixmaps_;
};

}  // namespace content

#endif  // CONTENT_COMMON_GPU_GPU_MEMORY_BUFFER_FACTORY_EFL_PIXMAP_H_
