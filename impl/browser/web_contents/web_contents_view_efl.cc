// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/logging.h"
#include "base/strings/utf_string_conversions.h"
#include "content/browser/web_contents/web_contents_impl.h"
#include "browser/web_contents/web_contents_view_efl.h"
#include "browser/renderer_host/render_widget_host_view_efl.h"
#include "eweb_context.h"
#include "eweb_view.h"
#include "web_contents_delegate_efl.h"
#include "content/public/browser/web_contents_view_delegate.h"

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
    , drag_dest_delegate_(NULL)
    , web_contents_(contents) {}

////////////////////////////////////////////////////////////////////////////////
// WebContentsViewEfl, WebContentsView implementation:

void WebContentsViewEfl::CreateView(const gfx::Size& initial_size,
                                    gfx::NativeView context) {
  requested_size_ = initial_size;

  if (delegate_)
    drag_dest_delegate_ = delegate_->GetDragDestDelegate();
}

RenderWidgetHostViewBase* WebContentsViewEfl::CreateViewForWidget(
    RenderWidgetHost* render_widget_host, bool is_guest_view_hack) {
  WebContentsDelegateEfl* delegate =
      static_cast<WebContentsDelegateEfl*>(web_contents_->GetDelegate());
  if (!delegate) {
    // If web_contents_ was created internally we do not have our delegate
    // set up yet. This is the case for popups. We will create
    // RenderWidgetHostView and the delegate when EWebView is created.
    return NULL;
  }

  RenderWidgetHostViewEfl* view = new RenderWidgetHostViewEfl(render_widget_host, delegate->web_view());
  view->Show();

  return view;
}

RenderWidgetHostViewBase* WebContentsViewEfl::CreateViewForPopupWidget(
    RenderWidgetHost* render_widget_host) {
  WebContentsDelegateEfl* delegate =
      static_cast<WebContentsDelegateEfl*>(web_contents_->GetDelegate());
  return new RenderWidgetHostViewEfl(render_widget_host, delegate->web_view());
}

void WebContentsViewEfl::SetPageTitle(const base::string16& title) {
  WebContentsDelegateEfl* delegate = static_cast<WebContentsDelegateEfl*>(web_contents_->GetDelegate());
  if (delegate)
    delegate->web_view()->SmartCallback<EWebViewCallbacks::TitleChange>().call(base::UTF16ToUTF8(title).c_str());
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
  NOTIMPLEMENTED();
}

void WebContentsViewEfl::RenderViewSwappedIn(RenderViewHost* host) {
  UpdateDragDest(host);
}

void WebContentsViewEfl::SetOverscrollControllerEnabled(bool enabled) {
  NOTIMPLEMENTED();
}

gfx::NativeView WebContentsViewEfl::GetNativeView() const {
  NOTIMPLEMENTED();
  return 0;
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

} // namespace content
