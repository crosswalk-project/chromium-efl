// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "browser/renderer_host/backing_store_efl.h"

#include "base/logging.h"
#include "base/md5.h"
#include "content/browser/renderer_host/dip_util.h"
#include "content/browser/renderer_host/render_process_host_impl.h"
#include "content/public/browser/render_widget_host.h"
#include "skia/ext/platform_canvas.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "ui/gfx/canvas.h"
#include "ui/gfx/image/image_skia.h"
#include "ui/gfx/rect.h"
#include "ui/gfx/rect_conversions.h"
#include "ui/gfx/size_conversions.h"
#include "ui/gfx/vector2d_conversions.h"

#include <Evas.h>

namespace {

gfx::Size ToPixelSize(gfx::Size dipSize, float scale)
{
  return gfx::ToCeiledSize(gfx::ScaleSize(dipSize, scale));
}

} // namespace

namespace content {

// See: BackingStoreAura.
static const int kMaxVideoLayerSize = 23170;

BackingStoreEfl::BackingStoreEfl(RenderWidgetHost* widget, Evas_Object* content_image, const gfx::Size& size)
    : BackingStore(widget, size)
    , content_image_(content_image)
    , device_scale_factor_(1.)
{
#if 0
  device_scale_factor_ =
    ui::GetScaleFactorScale(GetScaleFactorForView(widget->GetView()));
  gfx::Size pixel_size = ToPixelSize(size, device_scale_factor_);
#endif
  gfx::Size pixel_size = size;
  bitmap_.setConfig(SkBitmap::kARGB_8888_Config,
    pixel_size.width(), pixel_size.height());
  bitmap_.allocPixels();

  evas_object_image_colorspace_set(content_image_, EVAS_COLORSPACE_ARGB8888);
  if (CheckSizeConsistency())
    evas_object_image_data_set(content_image_, bitmap_.getAddr(0, 0));
}

BackingStoreEfl::~BackingStoreEfl()
{
}

void BackingStoreEfl::ScaleFactorChanged(float device_scale_factor)
{
  // FIXME: how to update content_image_ from scaled backing store?
  NOTIMPLEMENTED();
#if 0
  if (device_scale_factor == device_scale_factor_)
    return;

  gfx::Size old_pixel_size = ToPixelSize(size(), device_scale_factor_);
  device_scale_factor_ = device_scale_factor;

  gfx::Size pixel_size = ToPixelSize(size(), device_scale_factor_);
  SkBitmap new_bitmap;
  new_bitmap.setConfig(SkBitmap::kARGB_8888_Config,
    pixel_size.width(), pixel_size.height());
  new_bitmap.allocPixels();

  // Copy old contents; a low-res flash is better than a black flash.
  {
    scoped_ptr<SkCanvas> copy_canvas(new SkCanvas(new_bitmap));
    SkPaint copy_paint;
    copy_paint.setXfermodeMode(SkXfermode::kSrc_Mode);
    SkIRect src_rect = SkIRect::MakeWH(old_pixel_size.width(),
      old_pixel_size.height());
    SkRect dst_rect = SkRect::MakeWH(pixel_size.width(), pixel_size.height());
    copy_canvas.get()->drawBitmapRect(bitmap_, &src_rect, dst_rect,
      &copy_paint);
  }

  bitmap_ = new_bitmap;

  evas_object_image_size_set(content_image_, pixel_size.width(),
    pixel_size.height());
  evas_object_image_data_copy_set(content_image_, bitmap_.getAddr(0, 0));
#endif
}

size_t BackingStoreEfl::MemorySize()
{
#if 0
  return ToPixelSize(size(), device_scale_factor_).GetArea() * 4;
#endif
  return size().GetArea() * 4;
}

void BackingStoreEfl::PaintToBackingStore(
    RenderProcessHost* process,
    TransportDIB::Id bitmap,
    const gfx::Rect& bitmap_rect,
    const std::vector<gfx::Rect>& copy_rects,
    float scale_factor,
    const base::Closure& completion_callback,
    bool* scheduled_completion_callback)
{
  *scheduled_completion_callback = false;
  if (bitmap_rect.IsEmpty())
    return;

  if (!CheckSizeConsistency())
    return;

  gfx::Rect pixel_bitmap_rect = gfx::ToEnclosingRect(
    gfx::ScaleRect(bitmap_rect, scale_factor));

  const int width = pixel_bitmap_rect.width();
  const int height = pixel_bitmap_rect.height();

  if (width <= 0 || width > kMaxVideoLayerSize ||
      height <= 0 || height > kMaxVideoLayerSize)
    return;

  TransportDIB* dib = process->GetTransportDIB(bitmap);
  if (!dib)
    return;

  scoped_ptr<SkCanvas> copy_canvas(new SkCanvas(bitmap_));
  SkPaint copy_paint;
  copy_paint.setXfermodeMode(SkXfermode::kSrc_Mode);

  SkBitmap sk_bitmap;
  sk_bitmap.setConfig(SkBitmap::kARGB_8888_Config, width, height);
  sk_bitmap.setPixels(dib->memory());
  for (size_t i = 0; i < copy_rects.size(); i++) {
    const gfx::Rect pixel_copy_rect = gfx::ToEnclosingRect(
      gfx::ScaleRect(copy_rects[i], scale_factor));
    int x = pixel_copy_rect.x() - pixel_bitmap_rect.x();
    int y = pixel_copy_rect.y() - pixel_bitmap_rect.y();
    SkIRect srcrect = SkIRect::MakeXYWH(x, y,
      pixel_copy_rect.width(),
      pixel_copy_rect.height());

#if 0
    const gfx::Rect pixel_copy_dst_rect = gfx::ToEnclosingRect(
      gfx::ScaleRect(copy_rects[i], device_scale_factor_));
#endif
    const gfx::Rect pixel_copy_dst_rect = copy_rects[i];
    SkRect dstrect = SkRect::MakeXYWH(
      SkIntToScalar(pixel_copy_dst_rect.x()),
      SkIntToScalar(pixel_copy_dst_rect.y()),
      SkIntToScalar(pixel_copy_dst_rect.width()),
      SkIntToScalar(pixel_copy_dst_rect.height()));
    copy_canvas.get()->drawBitmapRect(sk_bitmap, &srcrect, dstrect,
      &copy_paint);

    evas_object_image_data_update_add(content_image_,
      pixel_copy_dst_rect.x(), pixel_copy_dst_rect.y(),
      pixel_copy_dst_rect.width(), pixel_copy_dst_rect.height());
  }

#ifdef DUMP_BACKING_STORE_IMAGE_HASH
  base::MD5Digest digest;
  base::MD5Sum(dib->memory(), width * height * 4, &digest);
  uint64* as_uint64 = (uint64*)(&digest.a[0]);
  uint64 low = as_uint64[0];
  uint64 high = as_uint64[1];
  LOG(ERROR) << "BackingStore image hash: " << low << " :: " << high;
#endif
#ifdef SAVE_EFL_BACKING_STORE_FRAMES
#define DIRNAME "/tmp/bs_efl"
  static char buf[256];
  static int count = 0;
  bool success = !mkdir(DIRNAME, 0777) || errno == EEXIST;
  if (success) {
    sprintf(buf, DIRNAME "/img%04d.png", count++);
    success = evas_object_image_save(content_image_, buf, 0, 0);
  }
  if (!success)
    LOG(ERROR) << "SAVE_EFL_BACKING_STORE_FRAMES failure";
#undef DIRNAME
#endif
}

void BackingStoreEfl::ScrollBackingStore(const gfx::Vector2d& delta,
                                         const gfx::Rect& clip_rect,
                                         const gfx::Size& view_size)
{
  if (!CheckSizeConsistency())
    return;

#if 0
  gfx::Rect pixel_rect = gfx::ToEnclosingRect(
    gfx::ScaleRect(clip_rect, device_scale_factor_));
  gfx::Vector2d pixel_delta = gfx::ToFlooredVector2d(
    gfx::ScaleVector2d(delta, device_scale_factor_));
#endif
  gfx::Rect pixel_rect = clip_rect;
  gfx::Vector2d pixel_delta = delta;

  // FIXME: this is from backing_store_aura.cc but the math seems strange.
  int x = std::min(pixel_rect.x(), pixel_rect.x() - pixel_delta.x());
  int y = std::min(pixel_rect.y(), pixel_rect.y() - pixel_delta.y());
  int w = pixel_rect.width() + abs(pixel_delta.x());
  int h = pixel_rect.height() + abs(pixel_delta.y());
  SkIRect rect = SkIRect::MakeXYWH(x, y, w, h);
  SkRegion invalidated;
  bitmap_.scrollRect(&rect, pixel_delta.x(), pixel_delta.y(), &invalidated);

  evas_object_image_data_update_add(content_image_, rect.x(), rect.y(),
                                    rect.width(), rect.height());
}

bool BackingStoreEfl::CopyFromBackingStore(const gfx::Rect& rect, skia::PlatformBitmap* output)
{
#if 0
  const int width =
    std::min(size().width(), rect.width()) * device_scale_factor_;
  const int height =
    std::min(size().height(), rect.height()) * device_scale_factor_;
#endif
  const int width = std::min(size().width(), rect.width());
  const int height = std::min(size().height(), rect.height());
  if (!output->Allocate(width, height, true))
    return false;

  SkIRect copy_rect = SkIRect::MakeXYWH(rect.x(), rect.y(), width, height);
  SkCanvas(output->GetBitmap()).writePixels(bitmap_, copy_rect.x(),
    copy_rect.y());

  return true;
}

#define LOG_SIZE_CONSISTENCY

bool BackingStoreEfl::CheckSizeConsistency() const
{
  int width, height;
  evas_object_image_size_get(content_image_, &width, &height);
  bool result = bitmap_.width() == width && bitmap_.height() == height;
#ifdef LOG_SIZE_CONSISTENCY
  if (!result)
    LOG(ERROR) << "BackingStoreEfl::CheckSizeConsistency() failed";
#endif
  return result;
}

} // namespace content
