// Copyright (c) 2012 The Chromium Authors. All rights reserved.
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
      const gfx::Size& initial_size, gfx::NativeView context) OVERRIDE;
  virtual RenderWidgetHostViewBase* CreateViewForWidget(
      RenderWidgetHost* render_widget_host) OVERRIDE;
  virtual RenderWidgetHostViewBase* CreateViewForPopupWidget(
      RenderWidgetHost* render_widget_host) OVERRIDE;
  virtual void SetPageTitle(const base::string16& title) OVERRIDE;
  virtual void RenderViewCreated(RenderViewHost* host) OVERRIDE;
  virtual void RenderViewSwappedIn(RenderViewHost* host) OVERRIDE;
  virtual void SetOverscrollControllerEnabled(bool enabled) OVERRIDE;
  virtual gfx::NativeView GetNativeView() const OVERRIDE;
  virtual gfx::NativeView GetContentNativeView() const OVERRIDE;
  virtual gfx::NativeWindow GetTopLevelNativeWindow() const OVERRIDE;
  virtual void GetContainerBounds(gfx::Rect* out) const OVERRIDE;
  virtual void SizeContents(const gfx::Size& size) OVERRIDE;
  virtual void Focus() OVERRIDE;
  virtual void SetInitialFocus() OVERRIDE;
  virtual void StoreFocus() OVERRIDE;
  virtual void RestoreFocus() OVERRIDE;
  virtual DropData* GetDropData() const OVERRIDE;
  virtual gfx::Rect GetViewBounds() const OVERRIDE;

  // content::RenderViewHostDelegateView implementation.
  virtual void ShowPopupMenu(const gfx::Rect& bounds,
                             int item_height,
                             double item_font_size,
                             int selected_item,
                             const std::vector<MenuItem>& items,
                             bool right_aligned,
                             bool allow_multiple_selection,
                             int advanced_ime_options = 0) OVERRIDE;

  virtual void ShowContextMenu(RenderFrameHost* render_frame_host, const ContextMenuParams& params) OVERRIDE;
  virtual void CancelContextMenu(int request_id) OVERRIDE;

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
