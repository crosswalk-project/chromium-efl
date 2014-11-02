// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Copyright 2014 Samsung Electronics. All rights reserved.
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
#include "third_party/WebKit/public/web/WebInputEvent.h"

#include <deque>
#include <Evas.h>
#include <Ecore_Evas.h>
#include <Evas_GL.h>
#include <Ecore_IMF_Evas.h>

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

class DisambiguationPopupEfl;
class IMContextEfl;
class RenderWidgetHostImpl;
class RenderWidgetHostView;
class ReadbackYUVInterface;

// RenderWidgetHostView class hierarchy described in render_widget_host_view.h.
class RenderWidgetHostViewEfl
  : public RenderWidgetHostViewBase,
    public ui::GestureConsumer,
    public ui::GestureEventHelper,
    public base::SupportsWeakPtr<RenderWidgetHostViewEfl>,
    public IPC::Sender {
 public:
  explicit RenderWidgetHostViewEfl(RenderWidgetHost*, EWebView*);

  // RenderWidgetHostViewBase implementation.
  virtual void InitAsChild(gfx::NativeView) override;
  virtual void InitAsPopup(content::RenderWidgetHostView*, const gfx::Rect&) override;
  virtual void InitAsFullscreen(content::RenderWidgetHostView*) override;
  virtual RenderWidgetHost* GetRenderWidgetHost() const override;
  virtual void SetSize(const gfx::Size&) override;
  virtual void SetBounds(const gfx::Rect&) override;
  virtual gfx::Vector2dF GetLastScrollOffset() const override;
  virtual gfx::NativeView GetNativeView() const override;
  virtual gfx::NativeViewId GetNativeViewId() const override;
  virtual gfx::NativeViewAccessible GetNativeViewAccessible() override;
  virtual bool IsSurfaceAvailableForCopy() const override;
  virtual void Show() override;
  virtual void Hide() override;
  virtual bool IsShowing() override;
  virtual gfx::Rect GetViewBounds() const override;
  virtual bool LockMouse() override;
  virtual void UnlockMouse() override;
  virtual void WasShown() override;
  virtual void WasHidden() override;
  virtual void Focus() override;
  virtual bool HasFocus() const override;
  void MovePluginContainer(const WebPluginGeometry&);
  virtual void MovePluginWindows(
      const std::vector<WebPluginGeometry>&) override;
  virtual void Blur() override;
  virtual void UpdateCursor(const WebCursor&) override;
  virtual void SetIsLoading(bool) override;
  virtual void TextInputStateChanged(
      const ViewHostMsg_TextInputState_Params& params) override;

  virtual void ImeCancelComposition() override;
  virtual void ImeCompositionRangeChanged(const gfx::Range&, const std::vector<gfx::Rect>&) override;
  virtual void FocusedNodeChanged(bool is_editable_node) override;

  virtual void Destroy() override;
  virtual void SetTooltipText(const base::string16&) override;
  virtual void SelectionChanged(const base::string16&, size_t, const gfx::Range&) override;
  virtual void SelectionBoundsChanged(const ViewHostMsg_SelectionBounds_Params&) override;
  virtual void CopyFromCompositingSurface(
    const gfx::Rect&,
    const gfx::Size& /* dst_size */,
    const base::Callback<void(bool, const SkBitmap&)>&,
    const SkColorType) override;

  virtual void CopyFromCompositingSurfaceToVideoFrame(
    const gfx::Rect&,
    const scoped_refptr<media::VideoFrame>&,
    const base::Callback<void(bool)>&) override;

  virtual bool CanCopyToVideoFrame() const override;
  virtual bool CanSubscribeFrame() const override;
  virtual void BeginFrameSubscription(
      scoped_ptr<RenderWidgetHostViewFrameSubscriber> subscriber) override;
  virtual void EndFrameSubscription() override;


#ifdef TIZEN_EDGE_EFFECT
  virtual void DidOverscroll(const DidOverscrollParams& params) override;
#endif

#ifdef TIZEN_CONTENTS_DETECTION
  virtual void OnContentsDetected(const char*) override;
#endif

  virtual void AcceleratedSurfaceInitialized(int, int) override;
  virtual void AcceleratedSurfaceBuffersSwapped(
    const GpuHostMsg_AcceleratedSurfaceBuffersSwapped_Params&,
    int) override;

  virtual void AcceleratedSurfacePostSubBuffer(
    const GpuHostMsg_AcceleratedSurfacePostSubBuffer_Params&,
    int) override;

  virtual void AcceleratedSurfaceSuspend() override;
  virtual void AcceleratedSurfaceRelease() override;
  virtual bool HasAcceleratedSurface(const gfx::Size&) override;
  virtual void GetScreenInfo(blink::WebScreenInfo*) override;
  virtual gfx::Rect GetBoundsInRootWindow() override;
  virtual gfx::GLSurfaceHandle GetCompositingSurface() override;
  virtual void ResizeCompositingSurface(const gfx::Size&);
  virtual void RenderProcessGone(base::TerminationStatus, int) override;
  virtual bool OnMessageReceived(const IPC::Message&) override;

  void OnFilteredMessageReceived(const IPC::Message&);

  virtual void ProcessAckedTouchEvent(const TouchEventWithLatencyInfo&, InputEventAckState) override;
  virtual void DidStopFlinging() override;

  virtual void ShowDisambiguationPopup(const gfx::Rect& rect_pixels,
                                       const SkBitmap& zoomed_bitmap) override;

  // ui::GestureEventHelper implementation.
  virtual bool CanDispatchToConsumer(ui::GestureConsumer* consumer) override;
  virtual void DispatchCancelTouchEvent(ui::TouchEvent* event) override;
  virtual void DispatchGestureEvent(ui::GestureEvent*) override;

  // IPC::Sender implementation:
  virtual bool Send(IPC::Message*) override;

  void OnSelectionTextStyleState(const SelectionStylePrams& params);
  void OnDidChangeMaxScrollOffset(int maxScrollX, int maxScrollY);

#ifdef OS_TIZEN
  void FilterInputMotion(const blink::WebGestureEvent& gesture_event);
  void makePinchZoom(void* eventInfo);
  void OnDidInputEventHandled(const blink::WebInputEvent* input_event, bool processed);
  void GetSnapshotForRect(gfx::Rect& rect);
  void SetRectSnapshot(const SkBitmap& bitmap);
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
  void OnWebAppCapableGet(bool capable, int callback_id);
  void OnWebAppIconUrlGet(const std::string &icon_url, int callback_id);
  void OnWebAppIconUrlsGet(const std::map<std::string, std::string> &icon_urls, int callback_id);
  void SelectRange(const gfx::Point& start, const gfx::Point& end);
  void MoveCaret(const gfx::Point& point);
  void OnMHTMLContentGet(const std::string& mhtml_content, int callback_id);

  Evas_GL_API* evasGlApi() { return evas_gl_api_; }
  gfx::Point ConvertPointInViewPix(gfx::Point point);
  gfx::Rect GetViewBoundsInPix() const;

  void OnTextInputInFormStateChanged(bool is_in_form_tag);
  void KeyUpEventQueuePush(int key) { keyupev_queue_.push(key); }
  void ClearQueues();

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

  Ecore_X_Window GetEcoreXWindow() const;

  void HandleCommitQueue(bool processed);
  void HandlePreeditQueue(bool processed);
  void HandleKeyUpQueue();
  void HandleKeyDownQueue();
  void SendCompositionKeyUpEvent(char c);

  void PaintTextureToSurface(GLuint texture_id);

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

  // The gesture recognizer for this view.
  // In Aura GestureRecognizer is global. Should we follow that?
  scoped_ptr<ui::GestureRecognizer> gesture_recognizer_;

  scoped_ptr<DisambiguationPopupEfl> disambiguation_popup_;

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
  int surface_id_;
  bool is_hw_accelerated_;
  bool is_modifier_key_;

  typedef std::queue<int> KeyUpEventQueue;
  KeyUpEventQueue keyupev_queue_;

  typedef std::queue<NativeWebKeyboardEvent*> KeyDownEventQueue;
  KeyDownEventQueue keydownev_queue_;

  // The last scroll offset of the view.
  gfx::Vector2dF last_scroll_offset_;

  DISALLOW_COPY_AND_ASSIGN(RenderWidgetHostViewEfl);
};

} // namespace content

#endif
