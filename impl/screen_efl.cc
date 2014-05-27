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

  virtual bool IsDIPEnabled() OVERRIDE {
    return false;
  }

  virtual gfx::Point GetCursorScreenPoint() OVERRIDE {
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

  virtual gfx::NativeWindow GetWindowUnderCursor() OVERRIDE {
    return GetWindowAtScreenPoint(GetCursorScreenPoint());
  }

  virtual gfx::NativeWindow GetWindowAtScreenPoint(const gfx::Point& point) OVERRIDE {
    return NULL;
  }

  virtual int GetNumDisplays() const OVERRIDE {
    return 1;
  }

  virtual std::vector<gfx::Display> GetAllDisplays() const OVERRIDE {
    return std::vector<gfx::Display>(1, GetPrimaryDisplay());
  }

  virtual gfx::Display GetDisplayNearestWindow(gfx::NativeView view) const OVERRIDE {
    return GetPrimaryDisplay();
  }

  virtual gfx::Display GetDisplayNearestPoint(
      const gfx::Point& point) const OVERRIDE {
    return GetPrimaryDisplay();
  }

  virtual gfx::Display GetDisplayMatching(
      const gfx::Rect& match_rect) const OVERRIDE {
    return GetPrimaryDisplay();
  }

  virtual gfx::Display GetPrimaryDisplay() const OVERRIDE {
    static ::Screen* screen = DefaultScreenOfDisplay(gfx::GetXDisplay());

    static gfx::Display display(0,
        gfx::Rect(0, 0, WidthOfScreen(screen), HeightOfScreen(screen)));
    return display;
  }

  virtual void AddObserver(gfx::DisplayObserver* observer) OVERRIDE {
  }

  virtual void RemoveObserver(gfx::DisplayObserver* observer) OVERRIDE {
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
