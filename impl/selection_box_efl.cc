/*
   Copyright (C) 2013 Samsung Electronics

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "eweb_view.h"
#include "selection_box_efl.h"

namespace content {

SelectionBoxEfl::SelectionBoxEfl(EWebView* parent_view)
  : status_(false),
    editable_(false),
    is_anchor_first_(true),
    is_caret_selection_(false),
    context_params_(new ContextMenuParams()),
    parent_view_(parent_view) {
}

void SelectionBoxEfl::UpdateHandleData() {
  // Swap the handlers when these handles cross over
  if (!is_anchor_first_) {
    gfx::Rect swap_rect_;
    swap_rect_ = left_rect_;
    left_rect_ = right_rect_;
    right_rect_ = swap_rect_;
  }
}

void SelectionBoxEfl::UpdateSelectStringData(const base::string16& text) {
  context_params_->selection_text = text;
}

void SelectionBoxEfl::UpdateRectData(const gfx::Rect& left_rect, const gfx::Rect& right_rect, bool is_anchor_first) {
  LOG(INFO) << __PRETTY_FUNCTION__ << " l_x = " << left_rect.x() << " l_y = " << left_rect.y() << " r_x = " << right_rect.x() << " r_y = " << right_rect.y();
  is_anchor_first_ = is_anchor_first;
  left_rect_ = left_rect;
  right_rect_ = right_rect;

  // Display point of context Menu
  Evas_Coord x, y;
  evas_object_geometry_get(parent_view_->evas_object(), &x, &y, 0, 0);
  //context params suppose to be global - related to evas not the web view
  context_params_->x = left_rect_.x() + x;
  context_params_->y = left_rect_.y() + y;
}

bool SelectionBoxEfl::IsInEditField() const {
  if (editable_ && !(left_rect_.width() && !(left_rect_.height()))) {
    LOG(INFO) << "SelectionBoxEfl::IsInEditField : true";
    return true;
  }
  LOG(INFO) << "SelectionBoxEfl::IsInEditField : false";
  return false;
}

}
