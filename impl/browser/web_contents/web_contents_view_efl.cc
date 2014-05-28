// Copyright (c) 2012 The Chromium Authors. All rights reserved.
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
  WebContentsViewEfl* view = new WebContentsViewEfl(web_contents);
  *render_view_host_delegate_view = view;
  return view;
}

WebContentsViewEfl::WebContentsViewEfl(WebContents* contents)
    : web_contents_(contents) {
}

////////////////////////////////////////////////////////////////////////////////
// WebContentsViewEfl, WebContentsView implementation:

void WebContentsViewEfl::CreateView(const gfx::Size& initial_size,
                                    gfx::NativeView context) {
  NOTIMPLEMENTED();
}

RenderWidgetHostViewBase* WebContentsViewEfl::CreateViewForWidget(
    RenderWidgetHost* render_widget_host) {
  WebContentsDelegateEfl* delegate =
      static_cast<WebContentsDelegateEfl*>(web_contents_->GetDelegate());
  if (!delegate) {
    // If web_contents_ was created internally we do not have our delegate
    // set up yet. This is the case for popups. We will create
    // RenderWidgetHostView and the delegate when EWebView is created.
    return NULL;
  }

  RenderWidgetHostViewEfl* view = new RenderWidgetHostViewEfl(render_widget_host);
  view->set_eweb_view(delegate->web_view());
  view->Show();

  return view;
}

RenderWidgetHostViewBase* WebContentsViewEfl::CreateViewForPopupWidget(
    RenderWidgetHost* render_widget_host) {
  return new RenderWidgetHostViewEfl(render_widget_host);
}

void WebContentsViewEfl::SetPageTitle(const base::string16& title) {
  WebContentsDelegateEfl* delegate = static_cast<WebContentsDelegateEfl*>(web_contents_->GetDelegate());
  if (delegate)
    delegate->web_view()->SmartCallback<EWebViewCallbacks::TitleChange>().call(base::UTF16ToUTF8(title).c_str());
}

void WebContentsViewEfl::RenderViewCreated(RenderViewHost* host) {
  NOTIMPLEMENTED();
}

void WebContentsViewEfl::RenderViewSwappedIn(RenderViewHost* host) {
  NOTIMPLEMENTED();
}

void WebContentsViewEfl::SetOverscrollControllerEnabled(bool enabled) {
  NOTIMPLEMENTED();
}

gfx::NativeView WebContentsViewEfl::GetNativeView() const {
  NOTIMPLEMENTED();
  return 0;
}

gfx::NativeView WebContentsViewEfl::GetContentNativeView() const {
  NOTIMPLEMENTED();
  return 0;
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

void WebContentsViewEfl::OnTabCrashed(base::TerminationStatus status, int error_code) {
  NOTIMPLEMENTED();
}

void WebContentsViewEfl::SizeContents(const gfx::Size& size) {
  NOTIMPLEMENTED();
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
void WebContentsViewEfl::ShowPopupMenu(const gfx::Rect& bounds,
                                       int item_height,
                                       double item_font_size,
                                       int selected_item,
                                       const std::vector<MenuItem>& items,
                                       bool right_aligned,
                                       bool allow_multiple_selection,
                                       int advanced_ime_options) {
  NOTIMPLEMENTED();
}

void WebContentsViewEfl::ShowContextMenu(const ContextMenuParams& params) {
  WebContentsDelegateEfl* delegate = static_cast<WebContentsDelegateEfl*>(web_contents_->GetDelegate());
  if (delegate)
    delegate->ShowContextMenu(params);
}

void WebContentsViewEfl::CancelContextMenu(int request_id) {
  WebContentsDelegateEfl* delegate = static_cast<WebContentsDelegateEfl*>(web_contents_->GetDelegate());
  if (delegate)
    delegate->CancelContextMenu(request_id);
}

} // namespace content
