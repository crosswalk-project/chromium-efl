// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/snapshot/snapshot.h"

#include "base/bind.h"
#include "base/callback.h"
#include "base/logging.h"
#include "base/numerics/safe_conversions.h"
#include "base/task_runner_util.h"
#include "ui/gfx/display.h"
#include "ui/gfx/image/image.h"
#include "ui/gfx/rect.h"
#include "ui/gfx/screen.h"

namespace ui {

bool GrabViewSnapshot(gfx::NativeView view,
                      std::vector<unsigned char>* png_representation,
                      const gfx::Rect& snapshot_bounds) {
  return GrabWindowSnapshot(view, png_representation, snapshot_bounds);
}

bool GrabWindowSnapshot(gfx::NativeWindow window,
                        std::vector<unsigned char>* png_representation,
                        const gfx::Rect& snapshot_bounds) {
  // Not supported in EFL port. Callers should fall back to the async version.
  return false;
}

void GrabWindowSnapshotAsync(
  gfx::NativeWindow window,
  const gfx::Rect& source_rect,
  scoped_refptr<base::TaskRunner> background_task_runner,
  const GrabWindowSnapshotAsyncPNGCallback& callback) {
  NOTIMPLEMENTED();
}

void GrabViewSnapshotAsync(
  gfx::NativeView view,
  const gfx::Rect& source_rect,
  scoped_refptr<base::TaskRunner> background_task_runner,
  const GrabWindowSnapshotAsyncPNGCallback& callback) {
  GrabWindowSnapshotAsync(view, source_rect, background_task_runner, callback);
}

}  // namespace ui
