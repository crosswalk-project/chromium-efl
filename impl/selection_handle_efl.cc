// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "eweb_view.h"
#include "selection_handle_efl.h"
#include "selection_controller_efl.h"

#include <Edje.h>
#include <Eina.h>
#include <Elementary.h>

namespace content {

// Size of the left/right margin which causes selection handles to be rotated
const int kReverseMargin = 32;
const char* kLeftHandlePath = "elm/entry/selection/block_handle_left";
const char* kRightHandlePath = "elm/entry/selection/block_handle_right";
const char* kInputHandlePath = "elm/entry/cursor_handle/default";

static const char* GetEdjeObjectGroupPath(SelectionHandleEfl::HandleType type) {
  switch (type) {
    case SelectionHandleEfl::HANDLE_TYPE_LEFT:
      return kLeftHandlePath;
    case SelectionHandleEfl::HANDLE_TYPE_RIGHT:
      return kRightHandlePath;
    case SelectionHandleEfl::HANDLE_TYPE_INPUT:
      return kInputHandlePath;
    default:
      return NULL;
  }
}

SelectionHandleEfl::SelectionHandleEfl(SelectionControllerEfl& controller, HandleType type, Evas_Object* parent)
  : base_point_(0,0),
    current_touch_point_(0,0),
    diff_point_(0,0),
    controller_(controller),
    pressed_(false),
    is_top_(false),
    handle_type_(type) {

  Evas* evas = evas_object_evas_get(parent);
  handle_ = edje_object_add(evas);

  const Eina_List* default_theme_list = elm_theme_list_get(0);
  const Eina_List* ittr;
  void* theme;
  EINA_LIST_FOREACH(default_theme_list, ittr, theme) {
    char* theme_path = elm_theme_list_item_path_get(static_cast<const char*>(theme), 0);
    if (theme_path) {
      edje_object_file_set(handle_, theme_path, GetEdjeObjectGroupPath(type));
      free(theme_path);
      break;
    }
  }

  edje_object_signal_emit(handle_, "edje,focus,in", "edje");
  edje_object_signal_emit(handle_, "elm,state,bottom", "elm");
  evas_object_smart_member_add(handle_, parent);
  evas_object_propagate_events_set(handle_, false);

  evas_object_event_callback_add(handle_, EVAS_CALLBACK_MOUSE_DOWN, OnMouseDown, this);
  evas_object_event_callback_add(handle_, EVAS_CALLBACK_MOUSE_UP, OnMouseUp, this);
}

SelectionHandleEfl::~SelectionHandleEfl() {
  evas_object_event_callback_del(handle_, EVAS_CALLBACK_MOUSE_DOWN, OnMouseDown);
  evas_object_event_callback_del(handle_, EVAS_CALLBACK_MOUSE_UP, OnMouseUp);
  evas_object_del(handle_);
}

void SelectionHandleEfl::Show() {
  evas_object_show(handle_);
}

void SelectionHandleEfl::Hide() {
  evas_object_hide(handle_);
}

bool SelectionHandleEfl::IsVisible() const {
  return evas_object_visible_get(handle_);
}

void SelectionHandleEfl::Move(const gfx::Point& point) {
  if (!pressed_)
    ChangeObjectDirection(CalculateDirection(point));
  MoveObject(point);
}

void SelectionHandleEfl::OnMouseDown(void* data, Evas*, Evas_Object*, void* event_info) {
  Evas_Event_Mouse_Down* event = static_cast<Evas_Event_Mouse_Down*>(event_info);
  SelectionHandleEfl* handle = static_cast<SelectionHandleEfl*>(data);
  handle->pressed_ = true;
  evas_object_event_callback_add(handle->handle_, EVAS_CALLBACK_MOUSE_MOVE,
      OnMouseMove, handle);
  //Save the diff_point between touch point and base point of the handle
  handle->diff_point_.SetPoint(event->canvas.x - handle->base_point_.x(),
      event->canvas.y - handle->base_point_.y());
  int delta_x = event->canvas.x - handle->diff_point_.x();
  int delta_y = event->canvas.y - handle->diff_point_.y();
  handle->base_point_.SetPoint(delta_x, delta_y);

  handle->controller_.OnMouseDown(
      gfx::Point(event->canvas.x , event->canvas.y ), handle->handle_type_);
}

void SelectionHandleEfl::OnMouseMove(void* data, Evas*, Evas_Object*, void* event_info) {
  Evas_Event_Mouse_Move* event = static_cast<Evas_Event_Mouse_Move*>(event_info);
  SelectionHandleEfl* handle = static_cast<SelectionHandleEfl*>(data);
  handle->current_touch_point_.SetPoint(event->cur.canvas.x, event->cur.canvas.y);
  ecore_job_add(UpdateMouseMove, handle);
}

void SelectionHandleEfl::OnMouseUp(void* data, Evas*, Evas_Object*, void* event_info) {
  Evas_Event_Mouse_Up* event = static_cast<Evas_Event_Mouse_Up*>(event_info);
  SelectionHandleEfl* handle = static_cast<SelectionHandleEfl*>(data);
  handle->pressed_ = false;
  evas_object_event_callback_del(handle->handle_, EVAS_CALLBACK_MOUSE_MOVE, OnMouseMove);

  Evas_Coord x, y;
  evas_object_geometry_get(handle->controller_.GetParentView()->evas_object(), &x, &y, 0, 0);
  handle->controller_.OnMouseUp(gfx::Point(event->canvas.x - x, event->canvas.y - y));
}

void SelectionHandleEfl::UpdateMouseMove(void* data) {
  SelectionHandleEfl* handle = static_cast<SelectionHandleEfl*>(data);

  int delta_x = handle->current_touch_point_.x() - handle->diff_point_.x();
  int delta_y = handle->current_touch_point_.y() - handle->diff_point_.y();
  handle->base_point_.SetPoint(delta_x, delta_y);
  handle->controller_.OnMouseMove(
      gfx::Point(handle->current_touch_point_.x(),
                 handle->current_touch_point_.y()), handle->handle_type_);
}

SelectionHandleEfl::HandleDirection SelectionHandleEfl::CalculateDirection(
    const gfx::Point& point) const {
  bool reverse_horizontally = false,  reverse_vertically = false;
  Evas_Coord x, y, deviceWidth, deviceHeight;
  int handleHeight;

  edje_object_part_geometry_get(handle_, "handle", 0, 0, 0, &handleHeight);
  evas_object_geometry_get(controller_.GetParentView()->evas_object(),
      &x, &y, &deviceWidth, &deviceHeight);
  gfx::Point conv_point(point.x() + x, point.y() + y);

  gfx::Rect reverse_direction_rect = gfx::Rect(x + kReverseMargin,
      y, deviceWidth - 2 * kReverseMargin, deviceHeight - handleHeight);

  if (!reverse_direction_rect.Contains(conv_point)) {
    if (conv_point.x() <= reverse_direction_rect.x() ||
        conv_point.x() >= reverse_direction_rect.right()) {
      reverse_vertically = true;
    }
    if (conv_point.y() <= reverse_direction_rect.y() ||
        conv_point.y() >= reverse_direction_rect.bottom()) {
      reverse_horizontally = true;
    }
  }

  if (handle_type_ != HANDLE_TYPE_INPUT) {
     if (reverse_vertically) {
       if (reverse_horizontally) {
         return DirectionTopReverse;
       } else {
         return DirectionBottomReverse;
       }
     } else {
       if (reverse_horizontally) {
         return DirectionTopNormal;
       } else {
         return DirectionBottomNormal;
       }
     }
  } else {
    // Input handle can only be rotated horizontally
    if (reverse_horizontally) {
      return DirectionTopNormal;
    } else {
      return DirectionBottomNormal;
    }
  }

  NOTREACHED();
}

void SelectionHandleEfl::ChangeObjectDirection(HandleDirection direction) {
  TRACE_EVENT2("selection,efl", __PRETTY_FUNCTION__,
               "handle type", handle_type_, "direction", direction);

  is_top_ = (direction == DirectionTopNormal || direction == DirectionTopReverse);

  switch (direction) {
    case DirectionBottomNormal:
      if (handle_type_ == HANDLE_TYPE_INPUT)
        edje_object_signal_emit(handle_, "edje,cursor,handle,show", "edje");
      else
        edje_object_signal_emit(handle_, "elm,state,bottom", "elm");
      break;
    case DirectionBottomReverse:
      if (handle_type_ == HANDLE_TYPE_INPUT)
        edje_object_signal_emit(handle_, "edje,cursor,handle,show", "edje");
      else
        edje_object_signal_emit(handle_, "elm,state,bottom,reversed", "elm");
      break;
    case DirectionTopNormal:
      if (handle_type_ == HANDLE_TYPE_INPUT)
        edje_object_signal_emit(handle_, "edje,cursor,handle,top", "edje");
      else
        edje_object_signal_emit(handle_, "elm,state,top", "elm");
      break;
    case DirectionTopReverse:
      if (handle_type_ == HANDLE_TYPE_INPUT)
        edje_object_signal_emit(handle_, "edje,cursor,handle,top", "edje");
      else
        edje_object_signal_emit(handle_, "elm,state,top,reversed", "elm");
      break;
    }

  switch (handle_type_) {
    case HANDLE_TYPE_LEFT:
      evas_object_smart_callback_call(controller_.GetParentView()->evas_object(),
        "selection,handle,left,direction", &direction);
      break;
    case HANDLE_TYPE_RIGHT:
      evas_object_smart_callback_call(controller_.GetParentView()->evas_object(),
        "selection,handle,right,direction", &direction);
      break;
    case HANDLE_TYPE_INPUT:
      evas_object_smart_callback_call(controller_.GetParentView()->evas_object(),
        "selection,handle,large,direction", &direction);
      break;
  }
}

gfx::Rect SelectionHandleEfl::GetSelectionRect() const {
  gfx::Rect rect;

  switch(handle_type_) {
  case HANDLE_TYPE_RIGHT:
    return controller_.GetRightRect();
  case HANDLE_TYPE_LEFT:
    return controller_.GetLeftRect();
  case HANDLE_TYPE_INPUT:
    // no rect for this type of handle
    break;
  }

  return gfx::Rect();
}

void SelectionHandleEfl::MoveObject(const gfx::Point& point) {
  Evas_Coord x, y;
  evas_object_geometry_get(controller_.GetParentView()->evas_object(), &x, &y,
      0, 0);
  if (handle_type_ == HANDLE_TYPE_INPUT && IsTop()) {
    evas_object_move(handle_, point.x() + x,
        point.y() + y - controller_.GetLeftRect().height());
  } else {
    evas_object_move(handle_, point.x() + x, point.y() + y);
  }
}

} // namespace content
