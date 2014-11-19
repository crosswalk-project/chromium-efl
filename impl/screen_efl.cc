// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "screen_efl.h"
#include "eweb_view.h"

#include <X11/Xlib.h>

#include "ui/gfx/screen_type_delegate.h"
#include "ui/base/x/x11_util.h"
#include "ui/gfx/display.h"
#include "ui/gfx/display_observer.h"
#include "ui/gfx/screen.h"
#include "ui/gfx/x/x11_types.h"
#include "ui/views/view.h"
#include "content/browser/renderer_host/dip_util.h"

namespace ui {

namespace {
// TODO: Get default values from platform
// http://107.108.218.239/bugzilla/show_bug.cgi?id=9662
#if defined(OS_TIZEN_MOBILE)
const float kDefaultDeviceScaleFactor = 2.0f;
#else
const float kDefaultDeviceScaleFactor = 1.0f;
#endif
} // namespace

class EflScreen : public gfx::Screen {
 public:
  EflScreen() {
  }

  virtual ~EflScreen() {
  }

  virtual gfx::Point GetCursorScreenPoint() override {
    XDisplay* display = gfx::GetXDisplay();

    ::Window root, child;
    int root_x, root_y, win_x, win_y;
    unsigned int mask;
    XQueryPointer(display,
                  DefaultRootWindow(display),
                  &root,
                  &child,
                  &root_x,
                  &root_y,
                  &win_x,
                  &win_y,
                  &mask);

    return gfx::Point(root_x, root_y);
  }

  virtual gfx::NativeWindow GetWindowUnderCursor() override {
    return GetWindowAtScreenPoint(GetCursorScreenPoint());
  }

  virtual gfx::NativeWindow GetWindowAtScreenPoint(const gfx::Point& point) override {
    return NULL;
  }

  virtual int GetNumDisplays() const override {
    return 1;
  }

  virtual std::vector<gfx::Display> GetAllDisplays() const override {
    return std::vector<gfx::Display>(1, GetPrimaryDisplay());
  }

  virtual gfx::Display GetDisplayNearestWindow(gfx::NativeView view) const override {
    return GetPrimaryDisplay();
  }

  virtual gfx::Display GetDisplayNearestPoint(
      const gfx::Point& point) const override {
    return GetPrimaryDisplay();
  }

  virtual gfx::Display GetDisplayMatching(
      const gfx::Rect& match_rect) const override {
    return GetPrimaryDisplay();
  }

  virtual gfx::Display GetPrimaryDisplay() const override {
    ::Screen* screen = DefaultScreenOfDisplay(gfx::GetXDisplay());

    const float device_scale_factor =
        gfx::Display::HasForceDeviceScaleFactor() ?
        gfx::Display::GetForcedDeviceScaleFactor() : kDefaultDeviceScaleFactor;

    gfx::Display display(0);

    // TODO(Youngsoo Choi): If there's a way to get orientation
    // from platform not using saved value, it would be better.
    display.SetRotationAsDegree(EWebView::GetOrientation());

    int width, height;
    if (display.rotation() == gfx::Display::ROTATE_90 ||
        display.rotation() == gfx::Display::ROTATE_270) {
      width = HeightOfScreen(screen);
      height = WidthOfScreen(screen);
    } else {
      width = WidthOfScreen(screen);
      height = HeightOfScreen(screen);
    }

    const gfx::Rect bounds_in_pixels = gfx::Rect(width, height);
    const gfx::Rect bounds_in_dip =
        content::ConvertRectToDIP(device_scale_factor, bounds_in_pixels);

    display.set_device_scale_factor(device_scale_factor);
    display.set_bounds(bounds_in_dip);

    return display;
  }

  virtual void AddObserver(gfx::DisplayObserver* observer) override {
  }

  virtual void RemoveObserver(gfx::DisplayObserver* observer) override {
  }

 private:
  DISALLOW_COPY_AND_ASSIGN(EflScreen);
};

void InstallScreenInstance()
{
  static bool installed = false;
  if (!installed) {
    installed = true;
    gfx::Screen::SetScreenInstance(gfx::SCREEN_TYPE_NATIVE, new ui::EflScreen());
  }
}
} // namespace ui

namespace gfx {
Screen* CreateNativeScreen() {
  return new ui::EflScreen();
}
} // namespace gfx
