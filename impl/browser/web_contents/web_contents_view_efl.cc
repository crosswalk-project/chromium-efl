// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "web_contents_view_efl.h"

#include "eweb_context.h"
#include "eweb_view.h"
#include "web_contents_delegate_efl.h"
#include "base/logging.h"
#include "base/strings/utf_string_conversions.h"
#include "browser/renderer_host/render_widget_host_view_efl.h"
#include "content/browser/web_contents/web_contents_impl.h"
#include "content/public/browser/web_contents_view_delegate.h"
#include "gl/gl_shared_context_efl.h"

namespace content {

WebContentsView* CreateWebContentsView(
    WebContentsImpl* web_contents,
    WebContentsViewDelegate* delegate,
    RenderViewHostDelegateView** render_view_host_delegate_view) {
  WebContentsViewEfl* view = new WebContentsViewEfl(web_contents, delegate);
  *render_view_host_delegate_view = view;
  return view;
}

WebContentsViewEfl::WebContentsViewEfl(WebContents* contents,
    WebContentsViewDelegate* delegate)
    : delegate_(delegate)
    , native_view_(NULL)
    , drag_dest_delegate_(NULL)
    , view_mode_(tizen_webview::TW_VIEW_MODE_WINDOWED) // default value for webkit
    , web_contents_(contents) {}

////////////////////////////////////////////////////////////////////////////////
// WebContentsViewEfl, WebContentsView implementation:

void WebContentsViewEfl::CreateView(const gfx::Size& initial_size,
                                    gfx::NativeView context) {
  requested_size_ = initial_size;
  native_view_ = static_cast<Evas_Object*>(context);
  GLSharedContextEfl::Initialize(native_view_);

  if (delegate_)
    drag_dest_delegate_ = delegate_->GetDragDestDelegate();
}

RenderWidgetHostViewBase* WebContentsViewEfl::CreateViewForWidget(
    RenderWidgetHost* render_widget_host, bool is_guest_view_hack) {
  RenderWidgetHostViewEfl* view = new RenderWidgetHostViewEfl(render_widget_host, GetEWebView());
  view->Init(native_view_);
  view->Show();

 if (view_mode_ != tizen_webview::TW_VIEW_MODE_WINDOWED) {
    view->SetViewMode(view_mode_);
 }
 
  return view;
}

RenderWidgetHostViewBase* WebContentsViewEfl::CreateViewForPopupWidget(
    RenderWidgetHost* render_widget_host) {
  return new RenderWidgetHostViewEfl(render_widget_host, GetEWebView());
}

void WebContentsViewEfl::SetPageTitle(const base::string16& title) {
  if (GetEWebView())
    GetEWebView()->SmartCallback<EWebViewCallbacks::TitleChange>().call(base::UTF16ToUTF8(title).c_str());
}

void WebContentsViewEfl::UpdateDragDest(RenderViewHost* host) {
  // Drag-and-drop is entirely managed by BrowserPluginGuest for guest
  // processes in a largely platform independent way. WebDragDestEfl
  // will result in spurious messages being sent to the guest process which
  // will violate assumptions.
  if (host->GetProcess() && host->GetProcess()->IsIsolatedGuest()) {
    DCHECK(!drag_dest_);
    return;
  }

  // If supporting RenderWidgetHostViewEfl::GetNativeView(), following lines
  // will be enabled similar to GTK+ port
  RenderWidgetHostViewEfl* view = static_cast<RenderWidgetHostViewEfl*>(
    web_contents_->GetRenderWidgetHostView());

  // If the host is already used by the drag_dest_, there's no point in deleting
  // the old one to create an identical copy.

  /*if (drag_dest_.get() && drag_dest_->widget() == content_view)
    return;*/

  // Clear the currently connected drag drop signals by deleting the old
  // drag_dest_ before creating the new one.
  drag_dest_.reset();
  // Create the new drag_dest_.
  drag_dest_.reset(new WebDragDestEfl(web_contents_, view));

  if (delegate_)
    drag_dest_->set_delegate(delegate_->GetDragDestDelegate());
}

void WebContentsViewEfl::RenderViewCreated(RenderViewHost* host) {
  DCHECK(host->GetProcess());
  DCHECK(web_contents_);
}

void WebContentsViewEfl::RenderViewSwappedIn(RenderViewHost* host) {
  UpdateDragDest(host);
}

void WebContentsViewEfl::SetOverscrollControllerEnabled(bool enabled) {
  NOTIMPLEMENTED();
}

gfx::NativeView WebContentsViewEfl::GetNativeView() const {
  return web_contents_->GetRenderWidgetHostView()->GetNativeView();
}

gfx::NativeView WebContentsViewEfl::GetContentNativeView() const {
  RenderWidgetHostView* rwhv = web_contents_->GetRenderWidgetHostView();
  if (!rwhv)
    return NULL;
  return rwhv->GetNativeView();
}

gfx::NativeWindow WebContentsViewEfl::GetTopLevelNativeWindow() const {
  NOTIMPLEMENTED();
  return 0;
}

void WebContentsViewEfl::GetContainerBounds(gfx::Rect* out) const {
  RenderWidgetHostViewEfl* view = static_cast<RenderWidgetHostViewEfl*>(
    web_contents_->GetRenderWidgetHostView());
  if (view)
    *out = view->GetBoundsInRootWindow();
}

void WebContentsViewEfl::SizeContents(const gfx::Size& size) {
  RenderWidgetHostView* rwhv = web_contents_->GetRenderWidgetHostView();
  if (rwhv)
    rwhv->SetSize(size);
}

void WebContentsViewEfl::Focus() {
  NOTIMPLEMENTED();
}

void WebContentsViewEfl::SetInitialFocus() {
  NOTIMPLEMENTED();
}

void WebContentsViewEfl::StoreFocus() {
  NOTIMPLEMENTED();
}

void WebContentsViewEfl::RestoreFocus() {
  NOTIMPLEMENTED();
}

DropData* WebContentsViewEfl::GetDropData() const {
  NOTIMPLEMENTED();
  return 0;
}

gfx::Rect WebContentsViewEfl::GetViewBounds() const {
  NOTIMPLEMENTED();
  return gfx::Rect();
}

////////////////////////////////////////////////////////////////////////////////
// WebContentsViewEfl, RenderViewHostDelegateView implementation:
void WebContentsViewEfl::ShowContextMenu(RenderFrameHost* render_frame_host, const ContextMenuParams& params) {
  if (delegate_)
    delegate_->ShowContextMenu(render_frame_host, params);
}

void WebContentsViewEfl::SetViewMode(tizen_webview::View_Mode mode) {
  view_mode_ = mode;
  RenderWidgetHostViewEfl* view = static_cast<RenderWidgetHostViewEfl*>(
                                    web_contents_->GetRenderWidgetHostView());
  if (view)
    view->SetViewMode(view_mode_);
}


#if defined(OS_MACOSX) || defined(OS_ANDROID) || defined(OS_TIZEN)
void WebContentsViewEfl::ShowPopupMenu(RenderFrameHost* render_frame_host,
                                       const gfx::Rect& bounds,
                                       int item_height,
                                       double item_font_size,
                                       int selected_item,
                                       const std::vector<MenuItem>& items,
                                       bool right_aligned,
                                       bool allow_multiple_selection) {
  WebContentsDelegateEfl* delegate =
      static_cast<WebContentsDelegateEfl*>(web_contents_->GetDelegate());
  if (delegate) {
    delegate->ShowPopupMenu(render_frame_host,
                            bounds,
                            blink::TextDirection(right_aligned),
                            0,
                            items,
                            0,
                            selected_item,
                            allow_multiple_selection);
  }
}
#endif

EWebView* WebContentsViewEfl::GetEWebView() const {
  // TODO: not best way, but until we merge 5486351bf10375c64ec997e613a2155ca3a24cc7
  // this is the only way to make it work. Fixup ca5b85c82985e02ee7e908952ba894ef7b7d2a32
  // should be merged too.
  WebContentsDelegateEfl* delegate =
      reinterpret_cast<WebContentsDelegateEfl*>(web_contents_->GetDelegate());

  if (delegate)
    return delegate->web_view();

  return NULL;
}

void WebContentsViewEfl::UpdateFormNavigation(int formElementCount,
    int currentNodeIndex, bool prevState, bool nextState) {
  WebContentsDelegateEfl* delegate = static_cast<WebContentsDelegateEfl*>(
      web_contents_->GetDelegate());
  if (delegate) {
    delegate->UpdateFormNavigation(formElementCount, currentNodeIndex,
        prevState, nextState);
  }
}

} // namespace content
