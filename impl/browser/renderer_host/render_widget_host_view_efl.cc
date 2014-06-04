// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "browser/renderer_host/render_widget_host_view_efl.h"

#include "base/auto_reset.h"
#include "base/basictypes.h"
#include "base/bind.h"
#include "base/callback_helpers.h"
#include "base/command_line.h"
#include "base/debug/trace_event.h"
#include "base/message_loop/message_loop.h"
#include "base/strings/string_number_conversions.h"

#include "base/logging.h"
#include "base/strings/utf_string_conversions.h"
#include "browser/renderer_host/im_context_efl.h"
#include "browser/renderer_host/scroll_detector.h"
#include "browser/renderer_host/web_event_factory_efl.h"
#include "content/browser/renderer_host/render_widget_host_impl.h"
#include "content/browser/renderer_host/ui_events_helper.h"
#include "content/browser/renderer_host/event_with_latency_info.h"
#include "content/public/browser/browser_thread.h"
#include "content/browser/renderer_host/dip_util.h"
#include "content/common/gpu/client/gl_helper.h"
#include "content/public/browser/render_widget_host_view_frame_subscriber.h"
#include "content/public/common/content_switches.h"
#include "content/common/view_messages.h"
#include "content/common/gpu/gpu_messages.h"
#include "common/render_messages_efl.h"
#include "eweb_context.h"
#include "media/base/video_util.h"
#include "selection_controller_efl.h"
#include "skia/ext/image_operations.h"
#include "third_party/WebKit/public/web/WebInputEvent.h"
#include "third_party/WebKit/public/platform/WebScreenInfo.h"
#include "third_party/WebKit/public/web/WebTouchPoint.h"
#include "ui/events/gestures/gesture_recognizer.h"
#include "ui/gfx/display.h"
#include "ui/gfx/rect.h"
#include "ui/gfx/screen.h"
#include "ui/views/widget/desktop_aura/desktop_screen.h"

#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Ecore_Input.h>
#include <Ecore_X.h>

namespace content {

void RenderWidgetHostViewBase::GetDefaultScreenInfo(blink::WebScreenInfo* results) {
  gfx::Screen* screen = gfx::Screen::GetNativeScreen();
  if (!screen)
    return;

  const gfx::Display display = screen->GetPrimaryDisplay();
  results->rect = display.bounds();
  results->availableRect = display.work_area();
  results->deviceScaleFactor = display.device_scale_factor();
  // TODO(derat|oshima): Don't hardcode this. Get this from display object.
  results->depth = 24;
  results->depthPerComponent = 8;
}

RenderWidgetHostViewEfl::RenderWidgetHostViewEfl(RenderWidgetHost* widget)
  : host_(RenderWidgetHostImpl::From(widget))
  , web_view_(NULL)
  , im_context_(NULL)
  , evas_(NULL)
  , content_image_(NULL)
  , scroll_detector_(new EflWebview::ScrollDetector()) {
    host_->SetView(this);
}

RenderWidgetHostViewEfl::~RenderWidgetHostViewEfl() {
  if (im_context_)
    delete im_context_;
}

void RenderWidgetHostViewEfl::set_eweb_view(EWebView* view) {
  web_view_ = view;
  evas_ = web_view_->GetEvas();
  DCHECK(evas_);

  content_image_ = web_view_->GetContentImageObject();
  DCHECK(content_image_);

  im_context_ = IMContextEfl::Create(this);
}

bool RenderWidgetHostViewEfl::OnMessageReceived(const IPC::Message& message) {
  bool handled = true;
  IPC_BEGIN_MESSAGE_MAP(RenderWidgetHostViewEfl, message)
    IPC_MESSAGE_HANDLER(ViewHostMsg_DidFirstVisuallyNonEmptyLayout, OnDidFirstVisuallyNonEmptyLayout)
    IPC_MESSAGE_HANDLER(EwkHostMsg_PlainTextGetContents, OnPlainTextGetContents)
    IPC_MESSAGE_HANDLER(EwkHostMsg_DidChangeContentsSize, OnDidChangeContentsSize)
    IPC_MESSAGE_HANDLER(EwkViewMsg_SelectionTextStyleState, OnSelectionTextStyleState)
    IPC_MESSAGE_HANDLER(EwkHostMsg_DidChangeMaxScrollOffset, OnDidChangeMaxScrollOffset)
    IPC_MESSAGE_HANDLER(EwkHostMsg_ReadMHTMLData, OnMHTMLContentGet)
    IPC_MESSAGE_HANDLER(EwkHostMsg_DidChangePageScaleFactor, OnDidChangePageScaleFactor)
    IPC_MESSAGE_HANDLER(EwkHostMsg_DidChangePageScaleRange, OnDidChangePageScaleRange)
    IPC_MESSAGE_UNHANDLED(handled = false)
  IPC_END_MESSAGE_MAP()
  return handled;
}

bool RenderWidgetHostViewEfl::Send(IPC::Message* message) {
  return host_->Send(message);
}

void RenderWidgetHostViewEfl::OnDidFirstVisuallyNonEmptyLayout() {
  web_view_->SmartCallback<EWebViewCallbacks::LoadNonEmptyLayoutFinished>().call();
}

void RenderWidgetHostViewEfl::OnSelectionTextStyleState(const SelectionStylePrams& params) {
  web_view_->OnQuerySelectionStyleReply(params);
}

void RenderWidgetHostViewEfl::InitAsChild(gfx::NativeView parent_view) {
  NOTIMPLEMENTED();
}

void RenderWidgetHostViewEfl::InitAsPopup(RenderWidgetHostView*, const gfx::Rect&) {
  NOTIMPLEMENTED();
}

void RenderWidgetHostViewEfl::InitAsFullscreen(RenderWidgetHostView*) {
  NOTIMPLEMENTED();
}

RenderWidgetHost* RenderWidgetHostViewEfl::GetRenderWidgetHost() const {
  return host_;
}

void RenderWidgetHostViewEfl::SetSize(const gfx::Size& size) {
  // FIXME: what is the purpose of this? Why would the engine resize us?
  NOTIMPLEMENTED();
}

void RenderWidgetHostViewEfl::SetBounds(const gfx::Rect& rect) {
  // FIXME: ditto.
  NOTIMPLEMENTED();
}

gfx::NativeView RenderWidgetHostViewEfl::GetNativeView() const {
  // With aura this is expected to return an aura::Window*.
  // We don't have that so make sure nobody calls this.
  //NOTREACHED();
  return gfx::NativeView();
}

gfx::NativeViewId RenderWidgetHostViewEfl::GetNativeViewId() const {
  Ecore_Evas* ee = ecore_evas_ecore_evas_get(evas_);
  return ecore_evas_window_get(ee);
}

gfx::NativeViewAccessible RenderWidgetHostViewEfl::GetNativeViewAccessible() {
  NOTIMPLEMENTED();
  return 0;
}

bool RenderWidgetHostViewEfl::IsSurfaceAvailableForCopy() const {
#warning "[M37] GetBackingStore does not exist. backing store removed from chromium"
  //return !!host_->GetBackingStore(false);
  return false;
}

void RenderWidgetHostViewEfl::Show() {
}

void RenderWidgetHostViewEfl::Hide() {
}

bool RenderWidgetHostViewEfl::IsShowing() {
  return evas_object_visible_get(content_image_);
}

gfx::Rect RenderWidgetHostViewEfl::GetViewBounds() const {
  int x, y, w, h;
  evas_object_geometry_get(content_image_, &x, &y, &w, &h);
  return gfx::Rect(x, y, w, h);
}

bool RenderWidgetHostViewEfl::LockMouse() {
  NOTIMPLEMENTED();
  return false;
}

void RenderWidgetHostViewEfl::UnlockMouse() {
  NOTIMPLEMENTED();
}

void RenderWidgetHostViewEfl::WasShown() {
  host_->WasShown();
}

void RenderWidgetHostViewEfl::WasHidden() {
  host_->WasHidden();
}

void RenderWidgetHostViewEfl::Focus() {
  web_view_->SetFocus(EINA_TRUE);
  host_->Focus();
}

bool RenderWidgetHostViewEfl::HasFocus() const {
  return web_view_->HasFocus();
}

void RenderWidgetHostViewEfl::MovePluginContainer(const WebPluginGeometry& move) {
  Ecore_X_Window surface_window = 0;
  PluginWindowToWidgetMap::const_iterator i = plugin_window_to_widget_map_.find(move.window);

  if (i != plugin_window_to_widget_map_.end())
    surface_window = i->second;

  if (!surface_window)
    return;

  if (!move.visible) {
    ecore_x_window_hide(surface_window);
    return;
  }

  ecore_x_window_show(surface_window);

  if (!move.rects_valid)
    return;

  ecore_x_window_move(surface_window, move.window_rect.x(), move.window_rect.y());
  ecore_x_window_resize(surface_window, move.window_rect.width(), move.window_rect.height());
}

void RenderWidgetHostViewEfl::MovePluginWindows(
    const std::vector<WebPluginGeometry>& moves) {
  for (size_t i = 0; i < moves.size(); i++)
    MovePluginContainer(moves[i]);
}

void RenderWidgetHostViewEfl::Blur() {
  host_->Blur();
}

void RenderWidgetHostViewEfl::UpdateCursor(const WebCursor&) {
  NOTIMPLEMENTED();
}

void RenderWidgetHostViewEfl::SetIsLoading(bool) {
  NOTIMPLEMENTED();
}

void RenderWidgetHostViewEfl::TextInputTypeChanged(ui::TextInputType type, ui::TextInputMode input_mode, bool can_compose_inline) {
  if (im_context_) {
    im_context_->UpdateInputMethodState(type, can_compose_inline, input_mode);
    // Make Empty Rect for inputFieldZoom Gesture
    // Finally, the empty rect is not used.
    gfx::Rect empty_rect = gfx::Rect(0, 0, 0, 0);
    host_->ScrollFocusedEditableNodeIntoRect(empty_rect);
  }

  //FIXME : Currently checking for input here. Later to be changed.
  // Currently in device the virtual keyboard is not appering. Once the IMF
  // implementation is in place set the selection content editable.
  SelectionControllerEfl* controller = web_view_->GetSelectionController();
  if (controller && controller->GetSelectionStatus())
    controller->SetSelectionEditable(true);
}

void RenderWidgetHostViewEfl::ImeCancelComposition() {
  if (im_context_)
    im_context_->CancelComposition();
}

bool RenderWidgetHostViewEfl::GetCompositionCharacterBounds(uint32 index, gfx::Rect* rect) const
{
  DCHECK(rect);
  if (index >= composition_character_bounds_.size())
    return false;

  *rect = composition_character_bounds_[index];
  return true;
}

void RenderWidgetHostViewEfl::ImeCompositionRangeChanged(
    const gfx::Range& range,
    const std::vector<gfx::Rect>& character_bounds) {
  SelectionControllerEfl* controller = web_view_->GetSelectionController();
  if (controller) {
    if(controller->GetCaretSelectionStatus()){
      controller->SetCaretSelectionStatus(false);
    }
  }

  composition_character_bounds_ = character_bounds;
}

void RenderWidgetHostViewEfl::DidUpdateBackingStore(
  const gfx::Rect& scroll_rect,
  const gfx::Vector2d& scroll_delta,
  const std::vector<gfx::Rect>& copy_rects,
  const std::vector<ui::LatencyInfo>& latency_info) {
  if (scroll_delta.x() || scroll_delta.y())
    scroll_detector_->OnChangeScrollOffset(web_view_, scroll_delta);
  // FIXME: Probably we should separate content_image from BackingStore
  // and set updates here.
}

void RenderWidgetHostViewEfl::Destroy() {
  delete this;
}

void RenderWidgetHostViewEfl::SetTooltipText(const base::string16& text) {
  web_view_->SmartCallback<EWebViewCallbacks::TooltipTextSet>().call(UTF16ToUTF8(text).c_str());
}

void RenderWidgetHostViewEfl::SelectionChanged(const base::string16& text,
  size_t offset,
  const gfx::Range& range) {
  SelectionControllerEfl* controller = web_view_->GetSelectionController();
  if (controller)
    controller->UpdateSelectionData(text);
}

void RenderWidgetHostViewEfl::SelectionBoundsChanged(
  const ViewHostMsg_SelectionBounds_Params& params) {
  if (im_context_)
    im_context_->UpdateCaretBounds(gfx::UnionRects(params.anchor_rect, params.focus_rect));

  SelectionControllerEfl* controller = web_view_->GetSelectionController();
  if (controller)
    controller->UpdateSelectionDataAndShow(params.anchor_rect, params.focus_rect, params.is_anchor_first);
}

void RenderWidgetHostViewEfl::ScrollOffsetChanged() {
  NOTIMPLEMENTED();
}

void RenderWidgetHostViewEfl::CopyFromCompositingSurface(
  const gfx::Rect& src_subrect,
  const gfx::Size& /* dst_size */,
  const base::Callback<void(bool, const SkBitmap&)>& callback,
  const SkColorType color_type) {
  // FIXME: should find a way to do it effectively.
#warning "[M37] host_ does not have GetSnapshotFromRenderer function anymore"
  // host_->GetSnapshotFromRenderer(src_subrect, callback);
}

// CopyFromCompositingSurfaceToVideoFrame implementation borrowed from Aura port
bool RenderWidgetHostViewEfl::CanSubscribeFrame() const {
  return true;
}

void RenderWidgetHostViewEfl::BeginFrameSubscription(
    scoped_ptr<RenderWidgetHostViewFrameSubscriber> subscriber) {
  frame_subscriber_ = subscriber.Pass();
}

void RenderWidgetHostViewEfl::EndFrameSubscription() {
  idle_frame_subscriber_textures_.clear();
  frame_subscriber_.reset();
}

void RenderWidgetHostViewEfl::ReturnSubscriberTexture(
    base::WeakPtr<RenderWidgetHostViewEfl> rwhvefl,
    scoped_refptr<OwnedMailbox> subscriber_texture,
    uint32 sync_point) {
  if (!subscriber_texture.get())
    return;
  if (!rwhvefl)
    return;
  DCHECK_NE(
      rwhvefl->active_frame_subscriber_textures_.count(subscriber_texture.get()),
      0u);

  subscriber_texture->UpdateSyncPoint(sync_point);

  rwhvefl->active_frame_subscriber_textures_.erase(subscriber_texture.get());
  if (rwhvefl->frame_subscriber_ && subscriber_texture->texture_id())
    rwhvefl->idle_frame_subscriber_textures_.push_back(subscriber_texture);
}

void RenderWidgetHostViewEfl::CopyFromCompositingSurfaceFinishedForVideo(
    base::WeakPtr<RenderWidgetHostViewEfl> rwhvefl,
    const base::Callback<void(bool)>& callback,
    scoped_refptr<OwnedMailbox> subscriber_texture,
    scoped_ptr<cc::SingleReleaseCallback> release_callback,
    bool result) {
  callback.Run(result);

  GLHelper* gl_helper = ImageTransportFactory::GetInstance()->GetGLHelper();
  uint32 sync_point = gl_helper ? gl_helper->InsertSyncPoint() : 0;
  if (release_callback) {
    // A release callback means the texture came from the compositor, so there
    // should be no |subscriber_texture|.
    DCHECK(!subscriber_texture);
    release_callback->Run(sync_point, false);
  }
  ReturnSubscriberTexture(rwhvefl, subscriber_texture, sync_point);
}

void RenderWidgetHostViewEfl::CopyFromCompositingSurfaceHasResultForVideo(
    base::WeakPtr<RenderWidgetHostViewEfl> rwhvefl,
    scoped_refptr<OwnedMailbox> subscriber_texture,
    scoped_refptr<media::VideoFrame> video_frame,
    const base::Callback<void(bool)>& callback,
    scoped_ptr<cc::CopyOutputResult> result) {
  base::ScopedClosureRunner scoped_callback_runner(base::Bind(callback, false));
  base::ScopedClosureRunner scoped_return_subscriber_texture(
      base::Bind(&ReturnSubscriberTexture, rwhvefl, subscriber_texture, 0));

  if (!rwhvefl)
    return;
  if (result->IsEmpty())
    return;
  if (result->size().IsEmpty())
    return;

  // Compute the dest size we want after the letterboxing resize. Make the
  // coordinates and sizes even because we letterbox in YUV space
  // (see CopyRGBToVideoFrame). They need to be even for the UV samples to
  // line up correctly.
  // The video frame's coded_size() and the result's size() are both physical
  // pixels.
  gfx::Rect region_in_frame =
      media::ComputeLetterboxRegion(gfx::Rect(video_frame->coded_size()),
                                    result->size());
  region_in_frame = gfx::Rect(region_in_frame.x() & ~1,
                              region_in_frame.y() & ~1,
                              region_in_frame.width() & ~1,
                              region_in_frame.height() & ~1);
  if (region_in_frame.IsEmpty())
    return;

  if (!result->HasTexture()) {
    DCHECK(result->HasBitmap());
    scoped_ptr<SkBitmap> bitmap = result->TakeBitmap();
    // Scale the bitmap to the required size, if necessary.
    SkBitmap scaled_bitmap;
    if (result->size().width() != region_in_frame.width() ||
        result->size().height() != region_in_frame.height()) {
      skia::ImageOperations::ResizeMethod method =
          skia::ImageOperations::RESIZE_GOOD;
      scaled_bitmap = skia::ImageOperations::Resize(*bitmap.get(), method,
                                                    region_in_frame.width(),
                                                    region_in_frame.height());
    } else {
      scaled_bitmap = *bitmap.get();
    }

    {
      SkAutoLockPixels scaled_bitmap_locker(scaled_bitmap);

      media::CopyRGBToVideoFrame(
          reinterpret_cast<uint8*>(scaled_bitmap.getPixels()),
          scaled_bitmap.rowBytes(),
          region_in_frame,
          video_frame.get());
    }
    ignore_result(scoped_callback_runner.Release());
    callback.Run(true);
    return;
  }

  ImageTransportFactory* factory = ImageTransportFactory::GetInstance();
  GLHelper* gl_helper = factory->GetGLHelper();
  if (!gl_helper)
    return;
  if (subscriber_texture.get() && !subscriber_texture->texture_id())
    return;

  cc::TextureMailbox texture_mailbox;
  scoped_ptr<cc::SingleReleaseCallback> release_callback;
  result->TakeTexture(&texture_mailbox, &release_callback);
  DCHECK(texture_mailbox.IsTexture());
  if (!texture_mailbox.IsTexture())
    return;

  gfx::Rect result_rect(result->size());

  content::ReadbackYUVInterface* yuv_readback_pipeline =
      rwhvefl->yuv_readback_pipeline_.get();
  if (yuv_readback_pipeline == NULL ||
      yuv_readback_pipeline->scaler()->SrcSize() != result_rect.size() ||
      yuv_readback_pipeline->scaler()->SrcSubrect() != result_rect ||
      yuv_readback_pipeline->scaler()->DstSize() != region_in_frame.size()) {
    GLHelper::ScalerQuality quality = GLHelper::SCALER_QUALITY_FAST;
    std::string quality_switch = switches::kTabCaptureDownscaleQuality;
    // If we're scaling up, we can use the "best" quality.
    if (result_rect.size().width() < region_in_frame.size().width() &&
        result_rect.size().height() < region_in_frame.size().height())
      quality_switch = switches::kTabCaptureUpscaleQuality;

    std::string switch_value =
        CommandLine::ForCurrentProcess()->GetSwitchValueASCII(quality_switch);
    if (switch_value == "fast")
      quality = GLHelper::SCALER_QUALITY_FAST;
    else if (switch_value == "good")
      quality = GLHelper::SCALER_QUALITY_GOOD;
    else if (switch_value == "best")
      quality = GLHelper::SCALER_QUALITY_BEST;

    rwhvefl->yuv_readback_pipeline_.reset(
        gl_helper->CreateReadbackPipelineYUV(quality,
                                             result_rect.size(),
                                             result_rect,
                                             video_frame->coded_size(),
                                             region_in_frame,
                                             true,
                                             true));
    yuv_readback_pipeline = rwhvefl->yuv_readback_pipeline_.get();
  }

  ignore_result(scoped_callback_runner.Release());
  ignore_result(scoped_return_subscriber_texture.Release());
  base::Callback<void(bool result)> finished_callback = base::Bind(
      &RenderWidgetHostViewEfl::CopyFromCompositingSurfaceFinishedForVideo,
      rwhvefl->AsWeakPtr(),
      callback,
      subscriber_texture,
      base::Passed(&release_callback));
  yuv_readback_pipeline->ReadbackYUV(
      texture_mailbox.mailbox(),
      texture_mailbox.sync_point(),
      video_frame,
      finished_callback);
}

// Efl port - Implementation done, will enable this function after getting video test site to verify
void RenderWidgetHostViewEfl::CopyFromCompositingSurfaceToVideoFrame(
  const gfx::Rect& src_subrect,
  const scoped_refptr<media::VideoFrame>& target,
  const base::Callback<void(bool)>& callback) {
  NOTIMPLEMENTED();
  callback.Run(false);
}

bool RenderWidgetHostViewEfl::CanCopyToVideoFrame() const {
#warning "[M37] host_ no longer has is_accelerated_compositing_active function"
  //return host_->is_accelerated_compositing_active();
  return false;
}

void RenderWidgetHostViewEfl::OnAcceleratedCompositingStateChange() {
  NOTIMPLEMENTED();
}

void RenderWidgetHostViewEfl::AcceleratedSurfaceInitialized(int host_id, int route_id) {
  // FIXME: new API in M34. need proper implementation.
  NOTIMPLEMENTED();
}

void RenderWidgetHostViewEfl::AcceleratedSurfaceBuffersSwapped(
  const GpuHostMsg_AcceleratedSurfaceBuffersSwapped_Params& params,
  int gpu_host_id) {
  AcceleratedSurfaceMsg_BufferPresented_Params ack_params;
  ack_params.sync_point = 0;
  RenderWidgetHostImpl::AcknowledgeBufferPresent(
    params.route_id, gpu_host_id, ack_params);
}

void RenderWidgetHostViewEfl::AcceleratedSurfacePostSubBuffer(
  const GpuHostMsg_AcceleratedSurfacePostSubBuffer_Params& params,
  int gpu_host_id) {
  NOTIMPLEMENTED();
}

void RenderWidgetHostViewEfl::AcceleratedSurfaceSuspend() {
  NOTIMPLEMENTED();
}

void RenderWidgetHostViewEfl::AcceleratedSurfaceRelease() {
  NOTIMPLEMENTED();
}

bool RenderWidgetHostViewEfl::HasAcceleratedSurface(const gfx::Size&) {
  return false;
}

void RenderWidgetHostViewEfl::GetScreenInfo(
    blink::WebScreenInfo* results) {
  return GetDefaultScreenInfo(results);
}

gfx::Rect RenderWidgetHostViewEfl::GetBoundsInRootWindow() {
  Ecore_Evas* ee = ecore_evas_ecore_evas_get(evas_);
  int x, y, w, h;
  ecore_evas_geometry_get(ee, &x, &y, &w, &h);
  return gfx::Rect(x, y, w, h);
}

gfx::GLSurfaceHandle RenderWidgetHostViewEfl::GetCompositingSurface() {
  NOTIMPLEMENTED();
  return gfx::GLSurfaceHandle();
}

void RenderWidgetHostViewEfl::SetHasHorizontalScrollbar(bool) {
  NOTIMPLEMENTED();
}

void RenderWidgetHostViewEfl::SetScrollOffsetPinning(bool, bool) {
  NOTIMPLEMENTED();
}

void RenderWidgetHostViewEfl::OnAccessibilityEvents(const std::vector<AccessibilityHostMsg_EventParams>&) {
  NOTIMPLEMENTED();
}

void RenderWidgetHostViewEfl::RenderProcessGone(base::TerminationStatus, int error_code) {
  // RenderWidgetHostImpl sets |view_| i.e. RenderWidgetHostViewEfl to NULL immediately after this call.
  // It expects RenderWidgetHostView to delete itself.
  // We only inform |web_view_| that renderer has crashed.
  // and in "process,crashed" callback, app is expected to delete the view.
  web_view_->set_renderer_crashed();
  Destroy();
}

void RenderWidgetHostViewEfl::HandleShow() {
  host_->WasShown();
}

void RenderWidgetHostViewEfl::HandleHide() {
  host_->WasHidden();
}

void RenderWidgetHostViewEfl::HandleResize(int width, int height) {
  host_->WasResized();
}

void RenderWidgetHostViewEfl::HandleFocusIn() {
  if (im_context_)
    im_context_->OnFocusIn();

  host_->SetActive(true);
  host_->GotFocus();
}

void RenderWidgetHostViewEfl::HandleFocusOut() {
  if (im_context_)
    im_context_->OnFocusOut();

  host_->SetActive(false);
  host_->LostCapture();
}

void RenderWidgetHostViewEfl::HandleEvasEvent(const Evas_Event_Mouse_Down* event) {
  host_->ForwardMouseEvent(WebEventFactoryEfl::toWebMouseEvent(web_view_->GetEvas(), web_view_->evas_object(), event));
}

void RenderWidgetHostViewEfl::HandleEvasEvent(const Evas_Event_Mouse_Up* event) {
  if (im_context_)
    im_context_->Reset();

  host_->ForwardMouseEvent(WebEventFactoryEfl::toWebMouseEvent(web_view_->GetEvas(), web_view_->evas_object(), event));
}

void RenderWidgetHostViewEfl::HandleEvasEvent(const Evas_Event_Mouse_Move* event) {
  host_->ForwardMouseEvent(WebEventFactoryEfl::toWebMouseEvent(web_view_->GetEvas(), web_view_->evas_object(), event));
}

void RenderWidgetHostViewEfl::HandleEvasEvent(const Evas_Event_Mouse_Wheel* event) {
  host_->ForwardWheelEvent(WebEventFactoryEfl::toWebMouseEvent(web_view_->GetEvas(), web_view_->evas_object(), event));
}

void RenderWidgetHostViewEfl::HandleEvasEvent(const Evas_Event_Key_Down* event) {
  bool wasFiltered = false;
  if (im_context_)
    im_context_->HandleKeyDownEvent(event, &wasFiltered);

  if(!wasFiltered)
    host_->ForwardKeyboardEvent(WebEventFactoryEfl::toWebKeyboardEvent(evas_, event));
}

void RenderWidgetHostViewEfl::HandleEvasEvent(const Evas_Event_Key_Up* event) {
  bool wasFiltered = false;
  if (im_context_)
    im_context_->HandleKeyUpEvent(event, &wasFiltered);

  if(!wasFiltered)
    host_->ForwardKeyboardEvent(WebEventFactoryEfl::toWebKeyboardEvent(evas_, event));
}

void RenderWidgetHostViewEfl::HandleGesture(ui::GestureEvent* event) {
  blink::WebGestureEvent gesture = content::MakeWebGestureEventFromUIEvent(*event);

  gesture.x = event->x();
  gesture.y = event->y();

  const gfx::Point root_point = event->root_location();
  gesture.globalX = root_point.x();
  gesture.globalY = root_point.y();

  if (event->type() == ui::ET_GESTURE_TAP_DOWN) {
    // Webkit does not stop a fling-scroll on tap-down. So explicitly send an
    // event to stop any in-progress flings.
    blink::WebGestureEvent fling_cancel = gesture;
    fling_cancel.type = blink::WebInputEvent::GestureFlingCancel;
    fling_cancel.sourceDevice = blink::WebGestureDeviceTouchscreen;
    host_->ForwardGestureEvent(fling_cancel);
  }

  if (gesture.type != blink::WebInputEvent::Undefined)
    host_->ForwardGestureEvent(gesture);

  event->SetHandled();
}

// Copied from render_widget_host_view_aura.cc
void UpdateWebTouchEventAfterDispatch(blink::WebTouchEvent* event,
  blink::WebTouchPoint* point) {
  if (point->state != blink::WebTouchPoint::StateReleased
      && point->state != blink::WebTouchPoint::StateCancelled)
    return;
  --event->touchesLength;
  for (unsigned i = point - event->touches;
       i < event->touchesLength;
       ++i)
    event->touches[i] = event->touches[i + 1];
}

void RenderWidgetHostViewEfl::HandleTouchEvent(ui::TouchEvent* event) {
  // Update the touch event first.
  blink::WebTouchPoint* point =
    content::UpdateWebTouchEventFromUIEvent(*event, &touch_event_);

  // Forward the touch event only if a touch point was updated, and there's a
  // touch-event handler in the page, and no other touch-event is in the queue.
  // It is important to always consume the event if there is a touch-event
  // handler in the page, or some touch-event is already in the queue, even if
  // no point has been updated, to make sure that this event does not get
  // processed by the gesture recognizer before the events in the queue.
  if (host_->ShouldForwardTouchEvent())
    event->StopPropagation();

  if (point) {
    if (host_->ShouldForwardTouchEvent()) {
      host_->ForwardTouchEventWithLatencyInfo(touch_event_, *event->latency());
    } else {
      // In case there was no touch event hander in the page we still want to feed the
      // event into GestureMapper to emulate mouse input. Without this the event would
      // be simply lost.
      TouchEventWithLatencyInfo eventWithLatencyInfo(touch_event_, *event->latency());
      ProcessAckedTouchEvent(eventWithLatencyInfo, INPUT_EVENT_ACK_STATE_NOT_CONSUMED);
    }
    UpdateWebTouchEventAfterDispatch(&touch_event_, point);
  }
}

void RenderWidgetHostViewEfl::ProcessAckedTouchEvent(const TouchEventWithLatencyInfo& touch,
    InputEventAckState ack_result) {
  DCHECK(eweb_view());
  eweb_view()->ProcessAckedTouchEvent(touch, ack_result);
}

void RenderWidgetHostViewEfl::OnPlainTextGetContents(const std::string& content_text, int plain_text_get_callback_id) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
  eweb_view()->InvokePlainTextGetCallback(content_text, plain_text_get_callback_id);
}

void RenderWidgetHostViewEfl::OnDidChangeContentsSize(int width, int height) {
  DCHECK(content::BrowserThread::CurrentlyOn(content::BrowserThread::UI));
  web_view_->DidChangeContentsSize(width, height);
}

void RenderWidgetHostViewEfl::OnOrientationChangeEvent(int orientation) {
  current_orientation_ = orientation;
}

void RenderWidgetHostViewEfl::OnDidChangeMaxScrollOffset(int maxScrollX, int maxScrollY) {
  scroll_detector_->SetMaxScroll(maxScrollX, maxScrollY);
}

void RenderWidgetHostViewEfl::SelectRange(const gfx::Point& start, const gfx::Point& end) {
#warning "[M37] SelectRange was moved somewhere, figure out how to access it."
#if 0
  host_->SelectRange(start, end);
#endif
}

void RenderWidgetHostViewEfl::MoveCaret(const gfx::Point& point) {
  host_->MoveCaret(point);
}

void RenderWidgetHostViewEfl::OnMHTMLContentGet(const std::string& mhtml_content, int callback_id) {
  eweb_view()->OnMHTMLContentGet(mhtml_content, callback_id);
}

void RenderWidgetHostViewEfl::OnDidChangePageScaleFactor(double scale_factor) {
  eweb_view()->DidChangePageScaleFactor(scale_factor);
}

void RenderWidgetHostViewEfl::OnDidChangePageScaleRange(double min_scale, double max_scale) {
  eweb_view()->DidChangePageScaleRange(min_scale, max_scale);
}

}  // namespace content
