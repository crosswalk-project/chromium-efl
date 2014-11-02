// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "screen_efl.h"

#include <X11/Xlib.h>

#include "ui/gfx/screen_type_delegate.h"
#include "ui/base/x/x11_util.h"
#include "ui/gfx/display.h"
#include "ui/gfx/display_observer.h"
#include "ui/gfx/screen.h"
#include "ui/gfx/x/x11_types.h"
#include "ui/views/view.h"

namespace ui {

class EflScreen : public gfx::Screen {
 public:
  EflScreen() {
  }

  virtual ~EflScreen() {
  }

  virtual bool IsDIPEnabled() override {
    return true;
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
    static ::Screen* screen = DefaultScreenOfDisplay(gfx::GetXDisplay());

    static gfx::Display display(0,
        gfx::Rect(0, 0, WidthOfScreen(screen), HeightOfScreen(screen)));

    //TODO:remove hardcoding
    if (!gfx::Display::HasForceDeviceScaleFactor())
#if defined(OS_TIZEN_MOBILE)
      display.set_device_scale_factor(2.0f);
#else
      display.set_device_scale_factor(1.0f);
#endif

    return display;
  }

  virtual void AddObserver(gfx::DisplayObserver* observer) override {
  }

  virtual void RemoveObserver(gfx::DisplayObserver* observer) override {
  }

 private:
  DISALLOW_COPY_AND_ASSIGN(EflScreen);
};

}

namespace content {
void InstallScreenInstance()
{
  static bool installed = false;
  if (!installed) {
    installed = true;
    gfx::Screen::SetScreenInstance(gfx::SCREEN_TYPE_NATIVE, new ui::EflScreen());
  }
}

}
