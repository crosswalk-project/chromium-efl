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

#ifndef selection_controller_h
#define selection_controller_h

#include "base/memory/scoped_ptr.h"
#include "base/strings/string16.h"
#include "ui/gfx/range/range.h"
#include "ui/gfx/image/image_skia.h"
#include "ui/gfx/rect.h"
#include "selection_box_efl.h"
#include "selection_handle_efl.h"
#include "selection_magnifier_efl.h"

#if defined(OS_TIZEN)
#include "vconf/vconf.h"
#endif

#include <libintl.h>

class EWebView;

namespace content {

// Controls the selection after long tap.
// This handles long tap down, long tap move, long tap up.
// On long tap down touch point is sent to engine by SendGestureEvent and magnifier is shown.
// On long tap move touch events are sent to engine by SelectClosestWord and magnifier is shown.
// On long tap up selection handlers are shown and context menu event is sent.
// Hnadlers are shown to extent selection, On handler move touch points are sent to engine
// by SelectRange to extend the selection
class SelectionControllerEfl {

  enum ContextMenuDirection {
    DirectionDown = 0,
    DirectionUp,
    DirectionLeft,
    DirectionRight,
    DirectionNone,
  };

 public:
  explicit SelectionControllerEfl(EWebView* parent_view);

  // Functions that handle long press, long press move and release
  void HandleLongPressEvent(const gfx::Point& touch_point);
  void HandleLongPressMoveEvent(const gfx::Point& touch_point);
  void HandleLongPressEndEvent();
  // Set if selection is valid
  void SetSelectionStatus(bool enable);
  bool GetSelectionStatus() const;
  // Set if selection is in edit field
  void SetSelectionEditable(bool enable);
  bool GetSelectionEditable() const;

  // To update the selection string
  void UpdateSelectionData(const base::string16& text);
  // To update the bitmap Image to show the magnifier
  void UpdateMagnifierScreen(Evas_Object* img);
  // To update the selection bounds
  void UpdateSelectionDataAndShow(const gfx::Rect& left_rect, const gfx::Rect& right_rect, bool is_anchor_first);
  void GetSelectionBounds(gfx::Rect* left, gfx::Rect* right);

  // Handles the mouse press,move and relase events on selection handles
  void OnMouseDown(const gfx::Point& touch_point);
  void OnMouseMove(const gfx::Point& touch_point, SelectionHandleEfl::HandleType);
  void OnMouseUp(const gfx::Point& touch_point);

  void SetCaretSelectionStatus(const bool enable);
  bool GetCaretSelectionStatus() const;

  // Clears the selection and hides context menu and handles
  void ClearSelection();
  void ClearSelectionViaEWebView();
  EWebView* GetParentView() { return parent_view_; }
  void HideHandle();
  void HideHandleAndContextMenu();
  bool IsAnyHandleVisible() const;

  void SetScrollStatus(const bool enable);
  bool GetScrollStatus();

  gfx::Rect GetLeftRect();
  gfx::Rect GetRightRect();

  void SetVisibilityBounds(const gfx::Rect& rect) { visibility_rect_ = rect; }

  void ChangeContextMenuPosition(gfx::Point& position, int& drawDirection);

  bool GetLongPressed() { return long_mouse_press_; }

  bool IsShowingMagnifier();

  Eina_Bool TextSelectionDown(int x, int y);
  Eina_Bool TextSelectionUp(int x, int y);

 private:
  void ShowHandleAndContextMenuIfRequired(bool anchor_first = true);
  void Clear();
  bool IsSelectionValid(const gfx::Rect& left_rect, const gfx::Rect& right_rect);

  static void EvasParentViewMoveCallback(void *data, Evas *e, Evas_Object *obj, void *event_info)
  { reinterpret_cast<SelectionControllerEfl*>(data)->OnParentParentViewMove(); }

#if defined(OS_TIZEN)
  static void PlatformLanguageChanged(keynode_t* keynode, void* data);
#endif

  void OnParentParentViewMove();

  // Is required to send back selction points and range extenstion co-ordinates
  EWebView* parent_view_;

  // Saves state so that context menu is not displayed during extending selection
  bool mouse_press_;

  // Saves state so that context menu is not displayed during page scrolling
  bool scrolling_;

  // True when new caret/selection position was sent to chromium,
  // but no reply was received, yet
  bool expecting_update_;

  // Saves state so that handlers and context menu is not shown when seletion change event occurs.
  bool long_mouse_press_;

  // Saves the data that are required to draw handle and context menu
  scoped_ptr<SelectionBoxEfl> selection_data_;

  // Points to start of the selection for extending selection
  scoped_ptr<SelectionHandleEfl> start_handle_;

  // Points to the end of the selection for extending selection
  scoped_ptr<SelectionHandleEfl> end_handle_;

  // Points to the caret in edit box where cursor is present
  scoped_ptr<SelectionHandleEfl> input_handle_;

  // Points to show the contents magnified
  scoped_ptr<SelectionMagnifierEfl> magnifier_;

  // Rectangle inside of which selection handles should be visible.
  gfx::Rect visibility_rect_;
};

}
#endif
