// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/common/gpu/gpu_memory_buffer_factory_efl_pixmap.h"

#include "ui/gl/efl_pixmap.h"
#include "ui/gl/gl_image_efl_pixmap.h"

namespace content {

GpuMemoryBufferFactoryEflPixmap::GpuMemoryBufferFactoryEflPixmap() {
}

GpuMemoryBufferFactoryEflPixmap::~GpuMemoryBufferFactoryEflPixmap() {
}

gfx::GpuMemoryBufferHandle
GpuMemoryBufferFactoryEflPixmap::CreateGpuMemoryBuffer(
    gfx::GpuMemoryBufferId id,
    const gfx::Size& size,
    unsigned internalformat,
    int client_id) {
  scoped_refptr<gfx::EflPixmap> efl_pixmap =
      gfx::EflPixmap::Create(gfx::EflPixmap::BUFFER, size);
  if (!efl_pixmap.get())
    return gfx::GpuMemoryBufferHandle();

  EflPixmapMapKey key(id, client_id);
  DCHECK(efl_pixmaps_.find(key) == efl_pixmaps_.end());
  efl_pixmaps_[key] = efl_pixmap;

  gfx::GpuMemoryBufferHandle handle;
  handle.type = gfx::EFL_PIXMAP_BUFFER;
  handle.id = id;
  handle.efl_pixmap_id = efl_pixmap->GetId();
  return handle;
}

void GpuMemoryBufferFactoryEflPixmap::DestroyGpuMemoryBuffer(
    gfx::GpuMemoryBufferId id,int client_id) {
  EflPixmapMapKey key(id, client_id);
  EflPixmapMap::iterator it = efl_pixmaps_.find(key);
  if (it != efl_pixmaps_.end())
    efl_pixmaps_.erase(it);
}

scoped_refptr<gfx::GLImage>
GpuMemoryBufferFactoryEflPixmap::CreateImageForGpuMemoryBuffer(
    unsigned long frame_pixmap_id,
    const gfx::Size& size,
    unsigned internalformat,
    int client_id ) {
  scoped_refptr<gfx::GLImageEflPixmap> image(new gfx::GLImageEflPixmap(size));
  if (!image->Initialize(frame_pixmap_id))
    return scoped_refptr<gfx::GLImage>();

  return image;
}

}  // namespace content
