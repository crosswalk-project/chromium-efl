// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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

  enum HandleDirection {
    DirectionBottomNormal,
    DirectionBottomReverse,
    DirectionTopNormal,
    DirectionTopReverse,
  };

  enum ContextMenuDirection {
    DirectionDown,
    DirectionUp,
    DirectionLeft,
    DirectionRight,
    DirectionNone,
  };

  SelectionHandleEfl(SelectionControllerEfl& controller, HandleType type, Evas_Object* parent);
  ~SelectionHandleEfl();
  void Show();
  void Hide();
  bool IsVisible() const;
  void Move(const gfx::Point& point);
  void SetBasePosition(const gfx::Point& point) { base_point_ = point; }
  gfx::Point GetBasePosition() const { return base_point_; }
  bool IsTop() { return is_top_; }
  Evas_Object* evas_object() const { return handle_; }

 private:
  static void OnMouseDown(void* data, Evas*, Evas_Object*, void* event_info);
  static void OnMouseMove(void* data, Evas*, Evas_Object*, void* event_info);
  static void OnMouseUp(void* data, Evas*, Evas_Object*, void* event_info);
  static void UpdateMouseMove(void* data);

  HandleDirection CalculateDirection(const gfx::Point&) const;
  void ChangeObjectDirection(HandleDirection direction);
  gfx::Rect GetSelectionRect() const;
  void MoveObject(const gfx::Point& point);

  // This point is one which will be used during extending selection
  // it is in web view coordinates
  gfx::Point base_point_;

  // This captures the handle move points
  // it is in evas coordinates
  gfx::Point current_touch_point_;

  // This save the gap between the touch point and base point when OnMouseDown is called
  gfx::Point diff_point_;

  // Parent to send back mouse events
  SelectionControllerEfl& controller_;

  // Handler object
  Evas_Object* handle_;

  // Is pressed
  bool pressed_;

  // Direction of handle
  bool is_top_;

  // Handle type
  HandleType handle_type_;
};

} // namespace
#endif
