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
#include "selection_handle_efl.h"
#include "selection_controller_efl.h"

#include <Edje.h>
#include <Eina.h>
#include <Elementary.h>

namespace content {

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

SelectionHandleEfl::SelectionHandleEfl(SelectionControllerEfl* controller, HandleType type, Evas_Object* parent)
  : base_point_(0,0),
    current_touch_point_(0,0),
    diff_point_(0,0),
    controller_(controller),
    is_cursor_handle_(false),
    is_mouse_downed_(false) {

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
  // FIXME: Currently no viewport boudry checks present. Add later
  evas_object_show(handle_);
}

void SelectionHandleEfl::Hide() {
  evas_object_hide(handle_);
}

bool SelectionHandleEfl::IsVisible() const {
  return evas_object_visible_get(handle_);
}

void SelectionHandleEfl::Move(const gfx::Point& point) {
  Evas_Coord x, y, deviceWidth, deviceHeight;
  gfx::Point movePoint = point;
  const int reverseMargin = 32;
  int handleHeight;
  edje_object_part_geometry_get(handle_, "handle", 0, 0, 0, &handleHeight);
  evas_object_geometry_get(controller_->GetParentView()->evas_object(), &x, &y, &deviceWidth, &deviceHeight);

  HandleType handleType = GetHandleType(point);
  LOG(INFO) << __PRETTY_FUNCTION__ << "HANDLE_TYPE : " << handleType;
  gfx::Rect selectionRect = GetSelectionRect(handleType);

  if ((handleType == HANDLE_TYPE_LEFT && (movePoint.x() <= reverseMargin)) 
      || (handleType == HANDLE_TYPE_RIGHT && (movePoint.x() >= deviceWidth - reverseMargin))) {
    if ((movePoint.y() + handleHeight) > (y + deviceHeight)) {
      movePoint.set_y(movePoint.y() - selectionRect.height());
      if (!is_mouse_downed_)
        ChangeObjectDirection(handleType, DirectionTopReverse);
    } else {
      if (!is_mouse_downed_)
        ChangeObjectDirection(handleType, DirectionBottomReverse);
    }
  } else {
    if ((movePoint.y() + handleHeight) > (y + deviceHeight)) {
      // Check If handler is not overLapping Device Boundary.
      if ((movePoint.y() - selectionRect.height() - handleHeight) >= y) {
        movePoint.set_y(movePoint.y() - selectionRect.height());
        if (!is_mouse_downed_) {
          if (is_cursor_handle_)
            ChangeObjectDirection(handleType, DirectionTopNormal);
          else
            ChangeObjectDirection(handleType, DirectionTopNormal);
        }
        MoveObject(movePoint);
        return;
      }
    }

    if (!is_mouse_downed_) {
      if (handleType == HANDLE_TYPE_INPUT)
        ChangeObjectDirection(handleType, DirectionBottomNormal);
      else
        ChangeObjectDirection(handleType, DirectionBottomNormal);
    }
  }

  MoveObject(movePoint);
}

void SelectionHandleEfl::OnMouseDown(void* data, Evas*, Evas_Object*, void* event_info) {
  Evas_Event_Mouse_Down* event = static_cast<Evas_Event_Mouse_Down*>(event_info);
  SelectionHandleEfl* handle = static_cast<SelectionHandleEfl*>(data);
  handle->SetIsMouseDowned(true);
  evas_object_event_callback_add(handle->handle_, EVAS_CALLBACK_MOUSE_MOVE, OnMouseMove, handle);
  //Save the diff_point between touch point and base point of the handle
  handle->diff_point_.SetPoint(event->canvas.x - handle->base_point_.x(), event->canvas.y - handle->base_point_.y());
  handle->controller_->OnMouseDown(gfx::Point(event->canvas.x , event->canvas.y ));
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
  handle->SetIsMouseDowned(false);
  evas_object_event_callback_del(handle->handle_, EVAS_CALLBACK_MOUSE_MOVE, OnMouseMove);

  Evas_Coord x, y;
  evas_object_geometry_get(handle->controller_->GetParentView()->evas_object(), &x, &y, 0, 0);
  handle->controller_->OnMouseUp(gfx::Point(event->canvas.x - x, event->canvas.y - y));
}

void SelectionHandleEfl::UpdateMouseMove(void* data) {
  SelectionHandleEfl* handle = static_cast<SelectionHandleEfl*>(data);
  Evas_Coord x, y;
  evas_object_geometry_get(handle->controller_->GetParentView()->evas_object(), &x, &y, 0, 0);
  if (!handle->is_cursor_handle_) {
    //New base point calculation should be based on new touch point and diff_point
    int delta_x = handle->current_touch_point_.x() - handle->diff_point_.x();
    int delta_y = handle->current_touch_point_.y() - handle->diff_point_.y();
    handle->base_point_.SetPoint(delta_x, delta_y);
    handle->controller_->OnMouseMove(gfx::Point(handle->current_touch_point_.x(), handle->current_touch_point_.y()), false);
  } else {
    handle->base_point_.set_x(handle->current_touch_point_.x() - x);
    handle->controller_->OnMouseMove(gfx::Point(handle->current_touch_point_.x() - x, handle->current_touch_point_.y() - y), true);
  }
}

void SelectionHandleEfl::ChangeObjectDirection(HandleType handleType, int direction) {
  LOG(INFO) << __PRETTY_FUNCTION__ << "HANDLE_TYPE : " << handleType << " direction = " << direction << " is_cursor_handle_ ? " << is_cursor_handle_;

  switch (direction) {
    case DirectionBottomNormal:
      if (is_cursor_handle_)
        edje_object_signal_emit(handle_, "edje,cursor,handle,show", "edje");
      else
        edje_object_signal_emit(handle_, "elm,state,bottom", "elm");
      break;
    case DirectionBottomReverse:
      if (is_cursor_handle_)
        edje_object_signal_emit(handle_, "edje,cursor,handle,show", "edje");
      else
        edje_object_signal_emit(handle_, "elm,state,bottom,reversed", "elm");
      break;
    case DirectionTopNormal:
      if (is_cursor_handle_)
        edje_object_signal_emit(handle_, "edje,cursor,handle,top", "edje");
      else
        edje_object_signal_emit(handle_, "elm,state,top", "elm");
      break;
    case DirectionTopReverse:
      if (is_cursor_handle_)
        edje_object_signal_emit(handle_, "edje,cursor,handle,top", "edje");
      else
        edje_object_signal_emit(handle_, "elm,state,top,reversed", "elm");
      break;
    }

  switch (handleType) {
    case HANDLE_TYPE_LEFT:
      evas_object_smart_callback_call(controller_->GetParentView()->evas_object(), "selection,handle,left,direction", &direction);
      break;
    case HANDLE_TYPE_RIGHT:
      evas_object_smart_callback_call(controller_->GetParentView()->evas_object(), "selection,handle,right,direction", &direction);
      break;
    case HANDLE_TYPE_INPUT:
      evas_object_smart_callback_call(controller_->GetParentView()->evas_object(), "selection,handle,large,direction", &direction);
      break;
  }
}

SelectionHandleEfl::HandleType SelectionHandleEfl::GetHandleType(const gfx::Point& point) {
  gfx::Rect leftRect = controller_->GetLeftRect();
  gfx::Rect rightRect = controller_->GetRightRect();
  HandleType type = HANDLE_TYPE_INPUT;

  if (leftRect.x() == rightRect.x())
    type = HANDLE_TYPE_INPUT;
  else if (point.x() == leftRect.x())
    type = HANDLE_TYPE_LEFT;
  else if (point.x() == rightRect.x())
    type = HANDLE_TYPE_RIGHT;

  return type;
}

gfx::Rect SelectionHandleEfl::GetSelectionRect(HandleType handleType) {
  gfx::Rect leftRect = controller_->GetLeftRect();
  gfx::Rect rect = leftRect;

  if (handleType == HANDLE_TYPE_RIGHT)
    rect = controller_->GetRightRect();

  return rect;
}

void SelectionHandleEfl::MoveObject(gfx::Point& point) {
  Evas_Coord x, y;
  evas_object_geometry_get(controller_->GetParentView()->evas_object(), &x, &y, 0, 0);
  evas_object_move(handle_, point.x() + x, point.y() + y);
}

}
