// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/common/gpu/client/gpu_memory_buffer_impl_efl_pixmap.h"

#include "base/atomic_sequence_num.h"
#include "base/bind.h"
#include "base/debug/trace_event.h"
#include "base/logging.h"
#include "base/numerics/safe_math.h"
#include "content/common/gpu/client/gpu_memory_buffer_factory_host.h"
#include "ui/gl/efl_pixmap.h"
#include "ui/gl/gl_bindings.h"

namespace content {
namespace {

// TODO(prashant.n): Move this to browser side code as per m41.
//base::StaticAtomicSequenceNumber g_next_buffer_id;


void GpuMemoryBufferDeleted(gfx::GpuMemoryBufferId id,
                            int client_id,
                            uint32 sync_point) {
  GpuMemoryBufferFactoryHost::GetInstance()->DestroyGpuMemoryBuffer(
      gfx::EFL_PIXMAP_BUFFER, id, client_id, sync_point);
}


void GpuMemoryBufferCreated(
    const gfx::Size& size,
    gfx::GpuMemoryBuffer::Format format,
    int client_id,
    const GpuMemoryBufferImpl::CreationCallback& callback,
    const gfx::GpuMemoryBufferHandle& handle) {
  if (handle.is_null()) {
    callback.Run(scoped_ptr<GpuMemoryBufferImpl>());
    return;
  }
  DCHECK_EQ(gfx::EFL_PIXMAP_BUFFER, handle.type);
  callback.Run(GpuMemoryBufferImplEflPixmap::CreateFromHandle(
      handle, size, format, base::Bind(&GpuMemoryBufferDeleted, handle.id, client_id)));
}

void GpuMemoryBufferCreatedForChildProcess(
    const GpuMemoryBufferImpl::AllocationCallback& callback,
    const gfx::GpuMemoryBufferHandle& handle) {
  DCHECK_EQ(gfx::EFL_PIXMAP_BUFFER, handle.type);
  callback.Run(handle);
}

}  // namespace

GpuMemoryBufferImplEflPixmap::GpuMemoryBufferImplEflPixmap(
    gfx::GpuMemoryBufferId id,
    const gfx::Size& size,
    Format format,
    const DestructionCallback& callback,
    scoped_ptr<gfx::EflPixmapImpl> efl_pixmap)
    : GpuMemoryBufferImpl(id, size, format, callback),
      efl_pixmap_(efl_pixmap.Pass()),
      stride_(0u) {
}//Need to check

GpuMemoryBufferImplEflPixmap::~GpuMemoryBufferImplEflPixmap() {
}

// static
void GpuMemoryBufferImplEflPixmap::Create(
    gfx::GpuMemoryBufferId id,
    const gfx::Size& size,
    Format format,
    int client_id,
    const CreationCallback& callback) {
  GpuMemoryBufferFactoryHost::GetInstance()->CreateGpuMemoryBuffer(
      gfx::EFL_PIXMAP_BUFFER,
      id,
      size,
      format,
      MAP,
      client_id,
      base::Bind(&GpuMemoryBufferCreated, size, format, client_id, callback));
}

// static
void GpuMemoryBufferImplEflPixmap::AllocateForChildProcess(
    gfx::GpuMemoryBufferId id,
    const gfx::Size& size,
    Format format,
    int child_client_id,
    const AllocationCallback& callback) {
  GpuMemoryBufferFactoryHost::GetInstance()->CreateGpuMemoryBuffer(
      gfx::EFL_PIXMAP_BUFFER,
      id,
      size,
      format,
      MAP,
      child_client_id,
      base::Bind(&GpuMemoryBufferCreatedForChildProcess, callback));
}
// static
scoped_ptr<GpuMemoryBufferImpl> GpuMemoryBufferImplEflPixmap::CreateFromHandle(
    const gfx::GpuMemoryBufferHandle& handle,
    const gfx::Size& size,
    Format format,
    const DestructionCallback& callback) {
  DCHECK(IsFormatSupported(format));
  DCHECK(handle.efl_pixmap_id);
  return make_scoped_ptr<GpuMemoryBufferImpl>(new GpuMemoryBufferImplEflPixmap(
             handle.id, size, format, callback,make_scoped_ptr<gfx::EflPixmapImpl>(new gfx::EflPixmapImpl(
                 gfx::EflPixmapBase::BUFFER, handle.efl_pixmap_id))));
}

void GpuMemoryBufferImplEflPixmap::DeletedByChildProcess(
    gfx::GpuMemoryBufferId id,
    int child_client_id,
    uint32_t sync_point) {
  GpuMemoryBufferFactoryHost::GetInstance()->DestroyGpuMemoryBuffer(
      gfx::EFL_PIXMAP_BUFFER, id, child_client_id, sync_point);
}

// static
bool GpuMemoryBufferImplEflPixmap::IsFormatSupported(Format format) {
  switch (format) {
    case RGBA_8888:
      return true;
    case RGBX_8888:
    case BGRA_8888:
      return false;
  }

  NOTREACHED();
  return false;
}

// static
bool GpuMemoryBufferImplEflPixmap::IsUsageSupported(Usage usage) {
  switch (usage) {
    case MAP:
      return true;
    case SCANOUT:
      return false;
  }

  NOTREACHED();
  return false;
}

// static
bool GpuMemoryBufferImplEflPixmap::IsLayoutSupported(const gfx::Size& size,
                                                     Format format) {
  if (!IsFormatSupported(format))
    return false;

  return true;
}

// static
bool GpuMemoryBufferImplEflPixmap::IsConfigurationSupported(Format format,
                                                            Usage usage) {
  return IsFormatSupported(format) && IsUsageSupported(usage);
}

void* GpuMemoryBufferImplEflPixmap::Map() {
  TRACE_EVENT0("gpu", "GpuMemoryBufferImplEflPixmap::Map");
  DCHECK(efl_pixmap_);
  DCHECK(!mapped_);
  mapped_ = true;
  void* buffer = efl_pixmap_->Map();
  stride_ = efl_pixmap_->GetStride();

  return buffer;
}

void GpuMemoryBufferImplEflPixmap::Unmap() {
  TRACE_EVENT0("gpu", "GpuMemoryBufferImplEflPixmap::Unmap");
  DCHECK(efl_pixmap_);
  DCHECK(mapped_);
  efl_pixmap_->UnMap();
  mapped_ = false;
}

uint32 GpuMemoryBufferImplEflPixmap::GetStride() const {
  return stride_;
}

gfx::GpuMemoryBufferHandle GpuMemoryBufferImplEflPixmap::GetHandle() const {
  gfx::GpuMemoryBufferHandle handle;
  handle.type = gfx::EFL_PIXMAP_BUFFER;
  handle.id = id_;
  handle.efl_pixmap_id = efl_pixmap_->GetId();
  return handle;
}

}  // namespace content
