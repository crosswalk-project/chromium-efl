// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "eweb_view.h"
#include "browser/renderer_host/scroll_detector.h"

namespace EflWebview {

ScrollDetector::ScrollDetector()
    : cached_is_main_frame_pinned_to_left_(false),
      cached_is_main_frame_pinned_to_right_(false),
      cached_is_main_frame_pinned_to_top_(false),
      cached_is_main_frame_pinned_to_bottom_(false),
      edge_Flag_(false) {
}

void ScrollDetector::OnChangeScrollOffset(EWebView* web_view, const gfx::Vector2d& scroll_delta) {
  edge_Flag_ = true;

  DetectEdge(web_view, scroll_delta);

  if (edge_Flag_ && scroll_delta.x() > 0)
    web_view->SmartCallback<EWebViewCallbacks::ScrollLeft>().call();
  if (edge_Flag_ && scroll_delta.x() < 0)
    web_view->SmartCallback<EWebViewCallbacks::ScrollRight>().call();
  if (edge_Flag_ && scroll_delta.y() < 0)
    web_view->SmartCallback<EWebViewCallbacks::ScrollDown>().call();
  if (edge_Flag_ && scroll_delta.y() > 0)
    web_view->SmartCallback<EWebViewCallbacks::ScrollUp>().call();
}

void ScrollDetector::DetectEdge(EWebView* web_view, const gfx::Vector2d& scroll_delta) {
  int x, y; //current scroll position
  web_view->GetScrollPosition(&x, &y);

  if (last_scroll_position_.x() != x)
    CheckForLeftRightEdges(web_view, x);

  if (last_scroll_position_.y() != y)
    CheckForTopBottomEdges(web_view, y);

  last_scroll_position_.set_x(x);
  last_scroll_position_.set_y(y);
}

void ScrollDetector::SetMaxScroll(int x, int y) {
  max_Scroll_.set_x(x);
  max_Scroll_.set_y(y);
}

void ScrollDetector::CheckForLeftRightEdges(EWebView* web_view, int scroll_x) {
  bool is_pinned_to_left = scroll_x <= min_Scroll_.x();
  bool is_pinned_to_right = scroll_x >= max_Scroll_.x();
  if (is_pinned_to_left && (is_pinned_to_left != cached_is_main_frame_pinned_to_left_)) {
    edge_Flag_ = false;
    web_view->SmartCallback<EWebViewCallbacks::EdgeLeft>().call();
  }

  if (is_pinned_to_right && (is_pinned_to_right != cached_is_main_frame_pinned_to_right_)) {
    edge_Flag_ = false;
    web_view->SmartCallback<EWebViewCallbacks::EdgeRight>().call();
  }

  cached_is_main_frame_pinned_to_left_ = is_pinned_to_left;
  cached_is_main_frame_pinned_to_right_ = is_pinned_to_right;
}

void ScrollDetector::CheckForTopBottomEdges(EWebView* web_view, int scroll_y) {
  bool is_pinned_to_top = scroll_y <= min_Scroll_.y();
  bool is_pinned_to_bottom = scroll_y >= max_Scroll_.y();
  if (is_pinned_to_top && (is_pinned_to_top != cached_is_main_frame_pinned_to_top_)) {
    edge_Flag_ = false;
    web_view->SmartCallback<EWebViewCallbacks::EdgeTop>().call();
  }

  if (is_pinned_to_bottom && (is_pinned_to_bottom != cached_is_main_frame_pinned_to_bottom_)) {
    edge_Flag_ = false;
    web_view->SmartCallback<EWebViewCallbacks::EdgeBottom>().call();
  }
  cached_is_main_frame_pinned_to_top_ = is_pinned_to_top;
  cached_is_main_frame_pinned_to_bottom_ = is_pinned_to_bottom;
}

}  // namespace EflWebview
