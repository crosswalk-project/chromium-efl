// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef RENDER_WIDGET_HOST_VIEW_EFL
#define RENDER_WIDGET_HOST_VIEW_EFL

#include "base/basictypes.h"
#include "base/format_macros.h"
#include "content/browser/renderer_host/render_widget_host_view_base.h"
#include "content/common/content_export.h"
#include "eweb_view.h"
#include <Evas.h>
#include <Ecore_Evas.h>

namespace ui {
class GestureEvent;
class TouchEvent;
}

namespace EflWebview {
class ScrollDetector;
}

namespace content {

class IMContextEfl;
class RenderWidgetHostImpl;
class RenderWidgetHostView;

// RenderWidgetHostView class hierarchy described in render_widget_host_view.h.
class RenderWidgetHostViewEfl : public RenderWidgetHostViewBase, public IPC::Sender {
 public:
  // RenderWidgetHostViewBase implementation.
  explicit RenderWidgetHostViewEfl(RenderWidgetHost*);

  virtual void InitAsChild(gfx::NativeView) OVERRIDE;
  virtual void InitAsPopup(content::RenderWidgetHostView*, const gfx::Rect&) OVERRIDE;
  virtual void InitAsFullscreen(content::RenderWidgetHostView*) OVERRIDE;
  virtual RenderWidgetHost* GetRenderWidgetHost() const OVERRIDE;
  virtual void SetSize(const gfx::Size&) OVERRIDE;
  virtual void SetBounds(const gfx::Rect&) OVERRIDE;
  virtual gfx::NativeView GetNativeView() const OVERRIDE;
  virtual gfx::NativeViewId GetNativeViewId() const OVERRIDE;
  virtual gfx::NativeViewAccessible GetNativeViewAccessible() OVERRIDE;
  virtual bool IsSurfaceAvailableForCopy() const OVERRIDE;
  virtual void Show() OVERRIDE;
  virtual void Hide() OVERRIDE;
  virtual bool IsShowing() OVERRIDE;
  virtual gfx::Rect GetViewBounds() const OVERRIDE;
  virtual bool LockMouse() OVERRIDE;
  virtual void UnlockMouse() OVERRIDE;
  virtual void WasShown() OVERRIDE;
  virtual void WasHidden() OVERRIDE;
  virtual void Focus() OVERRIDE;
  virtual bool HasFocus() const OVERRIDE;
  void MovePluginContainer(const WebPluginGeometry&);
  virtual void MovePluginWindows(
      const std::vector<WebPluginGeometry>&) OVERRIDE;
  virtual void Blur() OVERRIDE;
  virtual void UpdateCursor(const WebCursor&) OVERRIDE;
  virtual void SetIsLoading(bool) OVERRIDE;
  virtual void TextInputTypeChanged(ui::TextInputType, ui::TextInputMode, bool) OVERRIDE;

  virtual void ImeCancelComposition() OVERRIDE;
  virtual void ImeCompositionRangeChanged(const gfx::Range&, const std::vector<gfx::Rect>&) OVERRIDE;
  virtual void DidUpdateBackingStore(const gfx::Rect&,
    const gfx::Vector2d&,
    const std::vector<gfx::Rect>&,
    const std::vector<ui::LatencyInfo>&) OVERRIDE;

  virtual void Destroy() OVERRIDE;
  virtual void SetTooltipText(const base::string16&) OVERRIDE;
  virtual void SelectionChanged(const base::string16&, size_t, const gfx::Range&) OVERRIDE;
  virtual void SelectionBoundsChanged(const ViewHostMsg_SelectionBounds_Params&) OVERRIDE;
  virtual void ScrollOffsetChanged() OVERRIDE;
  virtual void CopyFromCompositingSurface(
    const gfx::Rect&,
    const gfx::Size& /* dst_size */,
    const base::Callback<void(bool, const SkBitmap&)>&,
    const SkColorType) OVERRIDE;

  virtual void CopyFromCompositingSurfaceToVideoFrame(
    const gfx::Rect&,
    const scoped_refptr<media::VideoFrame>&,
    const base::Callback<void(bool)>&) OVERRIDE;

  virtual bool CanCopyToVideoFrame() const OVERRIDE;
  virtual void OnAcceleratedCompositingStateChange() OVERRIDE;
  virtual void AcceleratedSurfaceInitialized(int, int) OVERRIDE;
  virtual void AcceleratedSurfaceBuffersSwapped(
    const GpuHostMsg_AcceleratedSurfaceBuffersSwapped_Params&,
    int) OVERRIDE;

  virtual void AcceleratedSurfacePostSubBuffer(
    const GpuHostMsg_AcceleratedSurfacePostSubBuffer_Params&,
    int) OVERRIDE;

  virtual void AcceleratedSurfaceSuspend() OVERRIDE;
  virtual void AcceleratedSurfaceRelease() OVERRIDE;
  virtual bool HasAcceleratedSurface(const gfx::Size&) OVERRIDE;
  virtual void GetScreenInfo(blink::WebScreenInfo*) OVERRIDE;
  virtual gfx::Rect GetBoundsInRootWindow() OVERRIDE;
  virtual gfx::GLSurfaceHandle GetCompositingSurface() OVERRIDE;
  virtual void SetHasHorizontalScrollbar(bool) OVERRIDE;
  virtual void SetScrollOffsetPinning(bool, bool) OVERRIDE;
  virtual void OnAccessibilityEvents(const std::vector<AccessibilityHostMsg_EventParams>&) OVERRIDE;
  virtual void RenderProcessGone(base::TerminationStatus, int) OVERRIDE;
  virtual bool OnMessageReceived(const IPC::Message&) OVERRIDE;
  virtual void ProcessAckedTouchEvent(const TouchEventWithLatencyInfo&, InputEventAckState) OVERRIDE;

  virtual bool IsEditingCommandEnabled(int) OVERRIDE { return false; }
  virtual void ExecuteEditingCommand(int) OVERRIDE {}
  // IPC::Sender implementation:
  virtual bool Send(IPC::Message*) OVERRIDE;

  void OnDidFirstVisuallyNonEmptyLayout();
  void OnSelectionTextStyleState(const SelectionStylePrams&);
  void OnDidChangeMaxScrollOffset(int, int);

  Evas* evas() const
  {
    DCHECK(evas_);
    return evas_;
  } 

  void set_eweb_view(EWebView*);
  EWebView* eweb_view() const { return web_view_; }
  RenderWidgetHostImpl* host() const { return host_; }

  void HandleShow();
  void HandleHide();
  void HandleMove(int, int) { }
  void HandleResize(int, int );
  void HandleFocusIn();
  void HandleFocusOut();

  void HandleEvasEvent(const Evas_Event_Mouse_Down*);
  void HandleEvasEvent(const Evas_Event_Mouse_Up*);
  void HandleEvasEvent(const Evas_Event_Mouse_Move*);
  void HandleEvasEvent(const Evas_Event_Mouse_Wheel*);
  void HandleEvasEvent(const Evas_Event_Key_Down*);
  void HandleEvasEvent(const Evas_Event_Key_Up*);
  void HandleGesture(ui::GestureEvent*);
  void HandleTouchEvent(ui::TouchEvent*);
  void OnPlainTextGetContents(const std::string&, int);
  void SelectRange(const gfx::Point&, const gfx::Point&);
  void MoveCaret(const gfx::Point&);
  void OnMHTMLContentGet(const std::string&, int);

 protected:
  friend class RenderWidgetHostView;

 private:
  virtual ~RenderWidgetHostViewEfl();

  void OnDidChangeContentsSize(int, int);
  void OnDidChangePageScaleFactor(double);
  void OnDidChangePageScaleRange(double, double);

  RenderWidgetHostImpl* host_;
  EWebView* web_view_;
  IMContextEfl* im_context_;

  Evas* evas_;
  Evas_Object* content_image_;
  scoped_ptr<EflWebview::ScrollDetector> scroll_detector_;

  typedef std::map<gfx::PluginWindowHandle, Ecore_X_Window> PluginWindowToWidgetMap;
  PluginWindowToWidgetMap plugin_window_to_widget_map_;

  // The touch-event. Its touch-points are updated as necessary. A new
  // touch-point is added from an ET_TOUCH_PRESSED event, and a touch-point is
  // removed from the list on an ET_TOUCH_RELEASED event.
  blink::WebTouchEvent touch_event_;

  DISALLOW_COPY_AND_ASSIGN(RenderWidgetHostViewEfl);
};

} // namespace content

#endif
