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
    controller_(controller),
    is_cursor_handle_(false) {
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

void SelectionHandleEfl::Move(const gfx::Point& point) {
  // FIXME: Currently no viewport boudry checks present. Add later
  // Also to check the handle direction based on the viewport
  Evas_Coord x, y, width, height;
  evas_object_geometry_get(controller_->GetParentView()->evas_object(), &x, &y, &width, &height);
  evas_object_move(handle_, point.x() + x, point.y() + y);
}

void SelectionHandleEfl::OnMouseDown(void* data, Evas*, Evas_Object*, void* event_info) {
  Evas_Event_Mouse_Down* event = static_cast<Evas_Event_Mouse_Down*>(event_info);
  SelectionHandleEfl* handle = static_cast<SelectionHandleEfl*>(data);
  evas_object_event_callback_add(handle->handle_, EVAS_CALLBACK_MOUSE_MOVE, OnMouseMove, handle);

  Evas_Coord x, y;
  evas_object_geometry_get(handle->controller_->GetParentView()->evas_object(), &x, &y, 0, 0);
  handle->controller_->OnMouseDown(gfx::Point(event->canvas.x - x, event->canvas.y - y));
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
    int height = 0;
    int width = 0;

    edje_object_part_geometry_get(handle->handle_, "handle", NULL, NULL, &width, &height);

    // The touch point on the handle will be approx middle of the handle.
    // So proportionally x and y are calculated and subtracted from the actual touch point
    // This will be the delta that is diff between old selection point the new one

    int delta_x = handle->current_touch_point_.x() - x - (width/2);
    int delta_y = handle->current_touch_point_.y() - y - (height/2);
    handle->base_point_.SetPoint(delta_x, delta_y);
    handle->controller_->OnMouseMove(gfx::Point(delta_x, delta_y), false);
  } else {
    handle->base_point_.set_x(handle->current_touch_point_.x() - x);
    handle->controller_->OnMouseMove(gfx::Point(handle->current_touch_point_.x() - x, handle->current_touch_point_.y() - y), true);
  }
}

}
