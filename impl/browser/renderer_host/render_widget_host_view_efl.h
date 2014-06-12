// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef RENDER_WIDGET_HOST_VIEW_EFL
#define RENDER_WIDGET_HOST_VIEW_EFL

#include "base/basictypes.h"
#include "base/format_macros.h"
#include "content/browser/renderer_host/render_widget_host_view_base.h"
#include "content/common/content_export.h"
#include "cc/layers/delegated_frame_provider.h"
#include "cc/layers/delegated_frame_resource_collection.h"
#include "cc/output/compositor_frame.h"
#include "cc/output/compositor_frame_ack.h"
#include "cc/output/copy_output_request.h"
#include "cc/output/copy_output_result.h"
#include "cc/resources/single_release_callback.h"
#include "cc/resources/texture_mailbox.h"
#include "content/browser/accessibility/browser_accessibility_manager.h"
#include "content/browser/compositor/image_transport_factory.h"
#include "content/browser/compositor/owned_mailbox.h"
#include "ui/base/ime/text_input_client.h"
#include "eweb_view.h"
#include "browser/renderer_host/im_context_efl.h"
#include <Evas.h>
#include <Ecore_Evas.h>
#include <Evas_GL.h>

#ifndef OS_TIZEN
// On desktops using mesa as GLES2 implementation GLchar is not defined
// Normally chromium uses it's own modified khronos headers from third_party/khronos,
// unfortunately those headers won't be included by Evas_GL.h
typedef char GLchar;
#endif

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
class ReadbackYUVInterface;

// RenderWidgetHostView class hierarchy described in render_widget_host_view.h.
class RenderWidgetHostViewEfl
  : public RenderWidgetHostViewBase,
    public ui::TextInputClient,
    public base::SupportsWeakPtr<RenderWidgetHostViewEfl>,
    public IPC::Sender {
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
  virtual void TextInputStateChanged(
      const ViewHostMsg_TextInputState_Params& params) OVERRIDE;

  void SaveImage(Evas_Object** img, const gfx::Rect& bounds);

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
  virtual bool CanSubscribeFrame() const OVERRIDE;
  virtual void BeginFrameSubscription(
      scoped_ptr<RenderWidgetHostViewFrameSubscriber> subscriber) OVERRIDE;
  virtual void EndFrameSubscription() OVERRIDE;

#ifdef TIZEN_EDGE_EFFECT
  virtual void OnOverscrolled(
      gfx::Vector2dF accumulated_overscroll,
      gfx::Vector2dF latest_overscroll_delta) OVERRIDE;
#endif

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
  virtual void ResizeCompositingSurface(const gfx::Size&);
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

  // Overridden from ui::TextInputClient implementation:
  virtual void SetCompositionText(
      const ui::CompositionText& composition) OVERRIDE {}
  virtual void ConfirmCompositionText() OVERRIDE {}
  virtual void ClearCompositionText() OVERRIDE {}
  virtual void InsertText(const base::string16& text) OVERRIDE {}
  virtual void InsertChar(base::char16 ch, int flags) OVERRIDE {}
  virtual gfx::NativeWindow GetAttachedWindow() const OVERRIDE { return gfx::NativeWindow(); }
  virtual ui::TextInputType GetTextInputType() const OVERRIDE { return ui::TextInputType(); }
  virtual ui::TextInputMode GetTextInputMode() const OVERRIDE { return ui::TextInputMode(); }
  virtual bool CanComposeInline() const OVERRIDE { return false; }
  virtual gfx::Rect GetCaretBounds() const OVERRIDE { return gfx::Rect(); }
  virtual bool GetCompositionCharacterBounds(uint32 index,
                                             gfx::Rect* rect) const OVERRIDE;
  virtual bool HasCompositionText() const OVERRIDE { return false; }
  virtual bool GetTextRange(gfx::Range* range) const OVERRIDE { return false; }
  virtual bool GetCompositionTextRange(gfx::Range* range) const OVERRIDE { return false; }
  virtual bool GetSelectionRange(gfx::Range* range) const OVERRIDE { return false; }
  virtual bool SetSelectionRange(const gfx::Range& range) OVERRIDE { return false; }
  virtual bool DeleteRange(const gfx::Range& range) OVERRIDE { return false; }
  virtual bool GetTextFromRange(const gfx::Range& range,
                                base::string16* text) const OVERRIDE { return false; }
  virtual void OnInputMethodChanged() OVERRIDE {}
  virtual bool ChangeTextDirectionAndLayoutAlignment(
      base::i18n::TextDirection direction) OVERRIDE { return false; }
  virtual void ExtendSelectionAndDelete(size_t before, size_t after) OVERRIDE {}
  virtual void EnsureCaretInRect(const gfx::Rect& rect) OVERRIDE {}
  virtual void OnCandidateWindowShown() OVERRIDE {}
  virtual void OnCandidateWindowUpdated() OVERRIDE {}
  virtual void OnCandidateWindowHidden() OVERRIDE {}
  virtual void DidStopFlinging() OVERRIDE;

  void OnDidFirstVisuallyNonEmptyLayout();
  void OnSelectionTextStyleState(const SelectionStylePrams& params);
  void OnDidChangeMaxScrollOffset(int maxScrollX, int maxScrollY);

#ifdef OS_TIZEN
  void FilterInputMotion(const blink::WebGestureEvent& gesture_event);
  void makePinchZoom(void* eventInfo);
#endif

  Evas* evas() const {
    DCHECK(evas_);
    return evas_;
  }

  void set_magnifier(bool status);

  void Init_EvasGL(int width, int height);

  void set_eweb_view(EWebView*);
  EWebView* eweb_view() const { return web_view_; }
  RenderWidgetHostImpl* host() const { return host_; }
  IMContextEfl* im_context() const { return im_context_; }

  float device_scale_factor() const { return device_scale_factor_; }

  void HandleShow();
  void HandleHide();
  void HandleMove(int, int) {}
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

  Evas_GL_API* evasGlApi() { return evas_gl_api_; }
  gfx::Point ConvertPointInViewPix(gfx::Point point);

  void OnTextInputInFormStateChanged(bool is_in_form_tag);

 protected:
  friend class RenderWidgetHostView;

 private:
  virtual ~RenderWidgetHostViewEfl();

  void OnDidChangeContentsSize(int, int);
  void OnOrientationChangeEvent(int);
  void OnDidChangePageScaleFactor(double);
  void OnDidChangePageScaleRange(double, double);

  SelectionControllerEfl* GetSelectionController();

  static void CopyFromCompositingSurfaceHasResultForVideo(
      base::WeakPtr<RenderWidgetHostViewEfl> rwhvefl,
      scoped_refptr<OwnedMailbox> subscriber_texture,
      scoped_refptr<media::VideoFrame> video_frame,
      const base::Callback<void(bool)>& callback,
      scoped_ptr<cc::CopyOutputResult> result);
  static void CopyFromCompositingSurfaceFinishedForVideo(
      base::WeakPtr<RenderWidgetHostViewEfl> rwhvefl,
      const base::Callback<void(bool)>& callback,
      scoped_refptr<OwnedMailbox> subscriber_texture,
      scoped_ptr<cc::SingleReleaseCallback> release_callback,
      bool result);
  static void ReturnSubscriberTexture(
      base::WeakPtr<RenderWidgetHostViewEfl> rwhvefl,
      scoped_refptr<OwnedMailbox> subscriber_texture,
      uint32 sync_point);

  static void EvasObjectImagePixelsGetCallback(void*, Evas_Object*);
  void initializeProgram();
  gfx::Rect GetViewBoundsInPix() const;

  Ecore_X_Window GetEcoreXWindow() const;

  RenderWidgetHostImpl* host_;
  EWebView* web_view_;
  IMContextEfl* im_context_;

  Evas* evas_;
  Evas_Object* content_image_;
  scoped_ptr<EflWebview::ScrollDetector> scroll_detector_;
  int m_IsEvasGLInit;
  float device_scale_factor_;

  typedef std::map<gfx::PluginWindowHandle, Ecore_X_Window> PluginWindowToWidgetMap;
  PluginWindowToWidgetMap plugin_window_to_widget_map_;

  // The current composition character bounds.
  std::vector<gfx::Rect> composition_character_bounds_;

  bool m_magnifier;

  // Whether we are currently loading.
  bool is_loading_;

  // YUV readback pipeline.
  scoped_ptr<content::ReadbackYUVInterface>
      yuv_readback_pipeline_;

  // Subscriber that listens to frame presentation events.
  scoped_ptr<RenderWidgetHostViewFrameSubscriber> frame_subscriber_;
  std::vector<scoped_refptr<OwnedMailbox> > idle_frame_subscriber_textures_;
  std::set<OwnedMailbox*> active_frame_subscriber_textures_;

  // The touch-event. Its touch-points are updated as necessary. A new
  // touch-point is added from an ET_TOUCH_PRESSED event, and a touch-point is
  // removed from the list on an ET_TOUCH_RELEASED event.
  blink::WebTouchEvent touch_event_;

  int current_orientation_;

  Evas_GL* evas_gl_;
  Evas_GL_API* evas_gl_api_;
  Evas_GL_Context* evas_gl_context_;
  Evas_GL_Surface* evas_gl_surface_;
  Evas_GL_Config* evas_gl_config_;

  GLuint program_id_;
  GLint source_texture_location_;
  GLuint position_attrib_;
  GLuint texcoord_attrib_;
  void* egl_image_;
  unsigned long current_pixmap_id_;
  unsigned long next_pixmap_id_;
  GLuint texture_id_;

  DISALLOW_COPY_AND_ASSIGN(RenderWidgetHostViewEfl);
};

} // namespace content

#endif
