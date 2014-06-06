// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BACKING_STORE_EFL
#define BACKING_STORE_EFL

#include "base/memory/scoped_ptr.h"
#include "content/browser/renderer_host/backing_store.h"
#include "third_party/skia/include/core/SkBitmap.h"

typedef struct _Evas_Object Evas_Object;

namespace gfx {
class Point;
class Canvas;
}

namespace content {
class RenderProcessHost;

class BackingStoreEfl : public BackingStore {
 public:
  BackingStoreEfl(RenderWidgetHost* widget, Evas_Object* content_image, const gfx::Size& size);
  virtual ~BackingStoreEfl();

  void ScaleFactorChanged(float device_scale_factor);

  // BackingStore implementation.
  virtual size_t MemorySize() OVERRIDE;

  virtual void PaintToBackingStore(
      RenderProcessHost* process,
      TransportDIB::Id bitmap,
      const gfx::Rect& bitmap_rect,
      const std::vector<gfx::Rect>& copy_rects,
      float scale_factor,
      const base::Closure& completion_callback,
      bool* scheduled_completion_callback) OVERRIDE;

  virtual bool CopyFromBackingStore(const gfx::Rect& rect, skia::PlatformBitmap* output) OVERRIDE;

  virtual void ScrollBackingStore(const gfx::Vector2d& delta,
                                  const gfx::Rect& clip_rect,
                                  const gfx::Size& view_size) OVERRIDE;

 private:
  // Check if bitmap_ has the same dimensions as content_image_
  bool CheckSizeConsistency() const;

  Evas_Object* content_image_;
  SkBitmap bitmap_;
  float device_scale_factor_;

  DISALLOW_COPY_AND_ASSIGN(BackingStoreEfl);
};

}  // namespace content

#endif
