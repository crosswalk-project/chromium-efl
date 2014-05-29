/*
 * Copyright (C) 2013 Samsung Electronics
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
*/

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
