// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _SCROLL_DETECTOR_H
#define _SCROLL_DETECTOR_H

#include "ui/gfx/rect.h"

class EWebView;

namespace EflWebview {

class ScrollDetector {
 public:
  ScrollDetector();
  void OnChangeScrollOffset(EWebView* web_view, const gfx::Vector2d& scroll_delta);
  void SetMaxScroll(int x, int y);

 private:
  void DetectEdge(EWebView* web_view, const gfx::Vector2d& scroll_delta);
  void CheckForLeftRightEdges(EWebView* web_view, int scroll_x);
  void CheckForTopBottomEdges(EWebView* web_view, int sroll_y);

  gfx::Vector2d last_scroll_position_;
  gfx::Vector2d max_Scroll_;
  gfx::Vector2d min_Scroll_;
  bool cached_is_main_frame_pinned_to_left_;
  bool cached_is_main_frame_pinned_to_right_;
  bool cached_is_main_frame_pinned_to_top_;
  bool cached_is_main_frame_pinned_to_bottom_;
  bool edge_Flag_;
};

}  // namespace EflWebview

#endif //_SCROLL_DETECTOR_H
