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

#ifndef selection_handle_h
#define selection_handle_h

#include "ui/gfx/point.h"

#include <Evas.h>

namespace content {

class SelectionControllerEfl;

// Controlls the display of handle.
// Handle marks the start/end of selection. In case of editor used to move the carret
// Listens on mouse up, down and move events.

class SelectionHandleEfl {
 public:
  enum HandleType {
    HANDLE_TYPE_LEFT,
    HANDLE_TYPE_RIGHT,
    HANDLE_TYPE_INPUT
  };

  SelectionHandleEfl(SelectionControllerEfl* controller, HandleType type, Evas_Object* parent);
  ~SelectionHandleEfl();
  void Show();
  void Hide();
  void Move(const gfx::Point& point);
  void SetBasePosition(const gfx::Point& point) { base_point_ = point; }
  gfx::Point GetBasePosition() const { return base_point_; }
  void SetCursorHandlerStatus(bool enable) { is_cursor_handle_ = enable; }

 private:
  static void OnMouseDown(void* data, Evas*, Evas_Object*, void* event_info);
  static void OnMouseMove(void* data, Evas*, Evas_Object*, void* event_info);
  static void OnMouseUp(void* data, Evas*, Evas_Object*, void* event_info);
  static void UpdateMouseMove(void* data);

  // This point is one which will be used during extending selection
  // it is in web view coordinates
  gfx::Point base_point_;

  // This captures the handle move points
  // it is in evas coordinates
  gfx::Point current_touch_point_;

  // Parent to send back mouse events
  content::SelectionControllerEfl* controller_;

  // Handler object
  Evas_Object* handle_;

  // Is set if the handle is of type input
  bool is_cursor_handle_;
};

} // namespace
#endif
