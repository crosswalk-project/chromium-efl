// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "web_drag_dest_efl.h"

#include <string>
#include "base/bind.h"
#include "base/files/file_path.h"
#include "base/message_loop/message_loop.h"
#include "base/strings/utf_string_conversions.h"
#include "content/browser/web_contents/web_contents_impl.h"
#include "content/public/browser/web_contents_delegate.h"
#include "content/public/browser/web_drag_dest_delegate.h"
#include "content/public/common/url_constants.h"
#include "net/base/net_util.h"
#include "third_party/WebKit/public/web/WebInputEvent.h"
#include "ui/base/clipboard/custom_data_helper.h"

#include "browser/renderer_host/render_widget_host_view_efl.h"

using blink::WebDragOperation;
using blink::WebDragOperationNone;
using blink::WebDragOperationCopy;

namespace content {

#ifdef OS_TIZEN_TV
static void drag_state_enter_cb(void *data, Evas_Object *obj) {
  WebDragDestEfl *webdragdestefl = (WebDragDestEfl *)data;

  webdragdestefl->drop_data_.reset(new DropData);

  if (webdragdestefl->delegate())
    webdragdestefl->delegate()->DragInitialize(webdragdestefl->web_contents());
}

static void drag_state_leave_cb(void *data, Evas_Object *obj) {
  WebDragDestEfl *webdragdestefl = (WebDragDestEfl *)data;

  base::MessageLoop::current()->PostTask(
    FROM_HERE,
    base::Bind(&WebDragDestEfl::DragLeave, webdragdestefl->method_factory_.GetWeakPtr()));
}

static void drag_Pos(void *data, Evas_Object *obj, Evas_Coord x, Evas_Coord y, Elm_Xdnd_Action action) {
  WebDragDestEfl *webdragdestefl = (WebDragDestEfl *)data;

  gfx::Rect rect = webdragdestefl->render_widget_host_view()->GetBoundsInRootWindow();
  gfx::Point point = gfx::Point(rect.x(), rect.y());

  webdragdestefl->GetRenderViewHost()->DragTargetDragOver(
      point,
      point,
      WebDragOperationCopy,
      blink::WebInputEvent::ControlKey);

  if (webdragdestefl->delegate())
    webdragdestefl->delegate()->OnDragOver();
}

static Eina_Bool drag_drop_cb(void *data, Evas_Object *obj, Elm_Selection_Data *drop) {
  WebDragDestEfl *webdragdestefl = (WebDragDestEfl *)data;
  webdragdestefl->drop_data_->text = base::NullableString16(base::UTF8ToUTF16(std::string(reinterpret_cast<const char*>(drop->data))),
                                     false);

  gfx::Rect rect = webdragdestefl->render_widget_host_view()->GetBoundsInRootWindow();
  gfx::Point point = gfx::Point(rect.x(), rect.y());

  webdragdestefl->GetRenderViewHost()->DragTargetDragEnter(
      *webdragdestefl->drop_data_.get(),
      point,
      point,
      WebDragOperationCopy,
      blink::WebInputEvent::ControlKey);

  if (webdragdestefl->delegate())
    webdragdestefl->delegate()->OnDragEnter();

  // Efl port need to check this, GTK+ port has drag data received & drag drop two callbacks
  webdragdestefl->method_factory_.InvalidateWeakPtrs();

  webdragdestefl->GetRenderViewHost()->
      DragTargetDrop(point, point, blink::WebInputEvent::ControlKey);

  if (webdragdestefl->delegate())
    webdragdestefl->delegate()->OnDrop();

  return EINA_TRUE;
}
#endif // OS_TIZEN_TV

WebDragDestEfl::WebDragDestEfl(WebContents* web_contents, RenderWidgetHostViewEfl *rwhvefl)
    : method_factory_(this),
      rwhvefl_(rwhvefl),
      web_contents_(web_contents) {
#ifdef OS_TIZEN_TV
  elm_drop_target_add(rwhvefl_->eweb_view()->evas_object(), ELM_SEL_FORMAT_TEXT,
                                   drag_state_enter_cb, this,
                                   drag_state_leave_cb, this,
                                   drag_Pos, this,
                                   drag_drop_cb, this);
#endif
}

WebDragDestEfl::~WebDragDestEfl() {
#ifdef OS_TIZEN_TV
  elm_drop_target_del(rwhvefl_->eweb_view()->evas_object());
#endif
}

void WebDragDestEfl::DragLeave() {
  GetRenderViewHost()->DragTargetDragLeave();
  if (delegate())
    delegate()->OnDragLeave();

  drop_data_.reset();
}

RenderViewHostImpl* WebDragDestEfl::GetRenderViewHost() const {
  return static_cast<RenderViewHostImpl*>(web_contents_->GetRenderViewHost());
}

}  // namespace content
