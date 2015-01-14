// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_BROWSER_WEB_CONTENTS_WEB_DRAG_DEST_EFL_H_
#define CONTENT_BROWSER_WEB_CONTENTS_WEB_DRAG_DEST_EFL_H_

#include "base/gtest_prod_util.h"
#include "base/memory/scoped_ptr.h"
#include "base/memory/weak_ptr.h"
#include "content/browser/renderer_host/render_view_host_impl.h"
#include "content/common/content_export.h"
#include "content/public/common/drop_data.h"
#include "third_party/WebKit/public/web/WebDragOperation.h"

#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Ecore_Input.h>
#include "ecore_x_wrapper.h"
#include <Elementary.h>

namespace content {

class RenderViewHostImpl;
class WebContents;
class WebDragDestDelegate;
class RenderWidgetHostViewEfl;

#define EFL_CONTENT_EXPORT __attribute__((visibility("default")))

class EFL_CONTENT_EXPORT WebDragDestEfl {
 public:
  WebDragDestEfl(WebContents* web_contents, RenderWidgetHostViewEfl* rwhvefl);
  ~WebDragDestEfl();

  // Informs the renderer when a system drag has left the render view.
  // See OnDragLeave().
  void DragLeave();

  WebDragDestDelegate* delegate() const { return delegate_; }
  void set_delegate(WebDragDestDelegate* delegate) { delegate_ = delegate; }

  RenderViewHostImpl* GetRenderViewHost() const;

  RenderWidgetHostViewEfl* render_widget_host_view() { return rwhvefl_; }

  WebContents* web_contents() { return web_contents_; }

  // EFL port - public variables for now, need to add get methods to make private,
  // faced build errors with scoped_ptr, base::WeakPtrFactory methods.

  // The data for the current drag, or NULL if |context_| is NULL.
  scoped_ptr<DropData> drop_data_;

  base::WeakPtrFactory<WebDragDestEfl> method_factory_;

 private:
  // A delegate that can receive drag information about drag events.
  WebDragDestDelegate* delegate_;

  // The render view.
  RenderWidgetHostViewEfl *rwhvefl_;

  WebContents* web_contents_;

  DISALLOW_COPY_AND_ASSIGN(WebDragDestEfl);
};
}  // namespace content

#endif  // CONTENT_BROWSER_WEB_CONTENTS_WEB_DRAG_DEST_EFL_H_
