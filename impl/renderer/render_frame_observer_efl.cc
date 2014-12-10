// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "renderer/render_frame_observer_efl.h"

#include "content/common/frame_messages.h"
#include "content/public/renderer/render_frame.h"
#include "content/public/renderer/render_view.h"
#include "content/renderer/render_frame_impl.h"
#include "content/renderer/external_popup_menu.h"
#include "common/render_messages_efl.h"
#include "third_party/WebKit/public/web/WebDocument.h"
#include "third_party/WebKit/public/web/WebElement.h"
#include "third_party/WebKit/public/web/WebLocalFrame.h"
#include "third_party/WebKit/public/web/WebView.h"

#include "base/logging.h"

using blink::WebDocument;
using blink::WebElement;
using blink::WebLocalFrame;

namespace content {

namespace {

WebElement GetFocusedElement(WebLocalFrame* frame) {
  //WebLocalFrame* frame_ = render_frame()->GetWebFrame();
  WebDocument doc = frame->document();
  if (!doc.isNull())
    return doc.focusedElement();

  return WebElement();
}

bool hasHTMLTagNameSelect(const WebElement& element) {
  if (element.isNull())
    return false;

  if (element.hasHTMLTagName("select"))
    return true;

  return false;
}

} // namespace

RenderFrameObserverEfl::RenderFrameObserverEfl(RenderFrame* render_frame)
    : content::RenderFrameObserver(render_frame) {
}

RenderFrameObserverEfl::~RenderFrameObserverEfl() {
}

bool RenderFrameObserverEfl::OnMessageReceived(const IPC::Message& message) {
  bool handled = true;
  IPC_BEGIN_MESSAGE_MAP(RenderFrameObserverEfl, message)
#if defined(OS_TIZEN)
    IPC_MESSAGE_HANDLER(FrameMsg_SelectPopupMenuItems, OnSelectPopupMenuItems)
    IPC_MESSAGE_HANDLER(FrameMsg_ClosePopupMenu, OnClosePopupMenu)
#endif
    IPC_MESSAGE_HANDLER(FrameMsg_MoveToPreviousSelectElement, OnMovePreviousSelectElement)
    IPC_MESSAGE_HANDLER(FrameMsg_MoveToNextSelectElement, OnMoveNextSelectElement)
    IPC_MESSAGE_UNHANDLED(handled = false)
  IPC_END_MESSAGE_MAP()
  return handled;
}

#if defined(OS_TIZEN)
void RenderFrameObserverEfl::OnSelectPopupMenuItems(
    bool canceled,
    const std::vector<int>& selected_indices) {
  RenderFrameImpl* render_frame_impl_ = static_cast<RenderFrameImpl*>(render_frame());
  ExternalPopupMenu* external_popup_menu_ = render_frame_impl_->GetExternalPopupMenu();
  if (external_popup_menu_ == NULL)
    return;
  // It is possible to receive more than one of these calls if the user presses
  // a select faster than it takes for the show-select-popup IPC message to make
  // it to the browser UI thread. Ignore the extra-messages.
  // TODO(jcivelli): http:/b/5793321 Implement a better fix, as detailed in bug.
  canceled = canceled || !hasHTMLTagNameSelect(GetFocusedElement(render_frame()->GetWebFrame()));
  external_popup_menu_->DidSelectItems(canceled, selected_indices);
  if (canceled)
    render_frame_impl_->DidHideExternalPopupMenu();
}

void RenderFrameObserverEfl::OnClosePopupMenu() {
  RenderFrameImpl* render_frame_impl_ = static_cast<RenderFrameImpl*>(render_frame());
  ExternalPopupMenu* external_popup_menu_ = render_frame_impl_->GetExternalPopupMenu();
  if (external_popup_menu_ == NULL)
    return;
  external_popup_menu_->close();
  render_frame_impl_->DidHideExternalPopupMenu();
}
#endif

void RenderFrameObserverEfl::OnMoveNextSelectElement() {
  content::RenderView* render_view_ = render_frame()->GetRenderView();
  render_view_->GetWebView()->moveSelectElementToNext();
}

void RenderFrameObserverEfl::OnMovePreviousSelectElement() {
  content::RenderView* render_view_ = render_frame()->GetRenderView();
  render_view_->GetWebView()->moveSelectElementToPrevious();
}

} // namespace content
