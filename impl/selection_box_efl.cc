// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "eweb_view.h"
#include "selection_box_efl.h"

namespace content {

SelectionBoxEfl::SelectionBoxEfl(EWebView* parent_view)
  : status_(false),
    editable_(false),
    is_caret_selection_(false),
    context_params_(new ContextMenuParams()),
    parent_view_(parent_view) {
}

void SelectionBoxEfl::UpdateSelectStringData(const base::string16& text) {
  context_params_->selection_text = text;
}

void SelectionBoxEfl::ClearRectData() {
  TRACE_EVENT0("selection,efl", __PRETTY_FUNCTION__);
  left_rect_ = right_rect_ = gfx::Rect(0, 0, 0, 0);
  context_params_->x = context_params_->y = 0;
}

void SelectionBoxEfl::UpdateRectData(const gfx::Rect& left_rect, const gfx::Rect& right_rect) {
  TRACE_EVENT2("selection,efl", __PRETTY_FUNCTION__,
               "left_rect", left_rect.ToString(),
               "right_rect", right_rect.ToString());
  if (left_rect < right_rect) {
    left_rect_ = left_rect;
    right_rect_ = right_rect;
  } else {
    right_rect_ = left_rect;
    left_rect_ = right_rect;
  }

  // Display point of context Menu
  Evas_Coord x, y;
  evas_object_geometry_get(parent_view_->evas_object(), &x, &y, 0, 0);
  //context params suppose to be global - related to evas not the web view
  context_params_->x = left_rect_.x() + x;
  context_params_->y = left_rect_.y() + y;
}

bool SelectionBoxEfl::IsInEditField() const {
  return (editable_ && !(left_rect_.width() && !(left_rect_.height())));
}

}
