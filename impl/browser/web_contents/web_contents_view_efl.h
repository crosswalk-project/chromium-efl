// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WEB_CONTENTS_VIEW_EFL
#define WEB_CONTENTS_VIEW_EFL

#include "browser/web_contents/web_drag_dest_efl.h"
#include "content/browser/renderer_host/render_view_host_delegate_view.h"
#include "content/browser/web_contents/web_contents_view.h"

#include <Evas.h>

class EWebView;

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
      RenderWidgetHost* render_widget_host, bool is_guest_view_hack) override;
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
#if defined(OS_MACOSX) || defined(OS_ANDROID) || defined(OS_TIZEN)
  virtual void ShowPopupMenu(RenderFrameHost* render_frame_host,
                             const gfx::Rect& bounds,
                             int item_height,
                             double item_font_size,
                             int selected_item,
                             const std::vector<MenuItem>& items,
                             bool right_aligned,
                             bool allow_multiple_selection) override;
#endif
  void UpdateDragDest(RenderViewHost* host);

  virtual void UpdateFormNavigation(int formElementCount, int currentNodeIndex,
      bool prevState, bool nextState) override;

private:
  EWebView* GetEWebView() const;

  // Our optional views wrapper.
  scoped_ptr<WebContentsViewDelegate> delegate_;

  // Native EFL widget responsible for handling input and drawing
  Evas_Object* native_view_;

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
