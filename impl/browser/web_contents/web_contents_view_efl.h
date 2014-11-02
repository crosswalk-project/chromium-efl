// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WEB_CONTENTS_VIEW_EFL
#define WEB_CONTENTS_VIEW_EFL

#include "browser/web_contents/web_drag_dest_efl.h"
#include "content/browser/renderer_host/render_view_host_delegate_view.h"
#include "content/browser/web_contents/web_contents_view.h"

namespace content {

class WebContents;
class WebContentsViewDelegate;

class WebContentsViewEfl
    : public content::WebContentsView
    , public content::RenderViewHostDelegateView {
 public:
  WebContentsViewEfl(WebContents* contents,
                     WebContentsViewDelegate* delegate = NULL);

  // content::WebContentsView implementation.
  virtual void CreateView(
      const gfx::Size& initial_size, gfx::NativeView context) override;
  virtual RenderWidgetHostViewBase* CreateViewForWidget(
      RenderWidgetHost* render_widget_host) override;
  virtual RenderWidgetHostViewBase* CreateViewForPopupWidget(
      RenderWidgetHost* render_widget_host) override;
  virtual void SetPageTitle(const base::string16& title) override;
  virtual void RenderViewCreated(RenderViewHost* host) override;
  virtual void RenderViewSwappedIn(RenderViewHost* host) override;
  virtual void SetOverscrollControllerEnabled(bool enabled) override;
  virtual gfx::NativeView GetNativeView() const override;
  virtual gfx::NativeView GetContentNativeView() const override;
  virtual gfx::NativeWindow GetTopLevelNativeWindow() const override;
  virtual void GetContainerBounds(gfx::Rect* out) const override;
  virtual void SizeContents(const gfx::Size& size) override;
  virtual void Focus() override;
  virtual void SetInitialFocus() override;
  virtual void StoreFocus() override;
  virtual void RestoreFocus() override;
  virtual DropData* GetDropData() const override;
  virtual gfx::Rect GetViewBounds() const override;

  // content::RenderViewHostDelegateView implementation.
  virtual void ShowContextMenu(RenderFrameHost* render_frame_host, const ContextMenuParams& params) override;

  void UpdateDragDest(RenderViewHost* host);

  // Our optional views wrapper.
  scoped_ptr<WebContentsViewDelegate> delegate_;

  WebDragDestDelegate* drag_dest_delegate_;

  // The size we want the view to be.
  gfx::Size requested_size_;

  // The helper object that handles drag destination related interactions with
  // EFL.
  scoped_ptr<WebDragDestEfl> drag_dest_;

  WebContents* web_contents_;
};

}

#endif
