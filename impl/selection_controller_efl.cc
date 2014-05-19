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
#include "selection_controller_efl.h"
#include "selection_handle_efl.h"
#include "selection_box_efl.h"
#include "selection_magnifier_efl.h"

#include <Edje.h>

namespace content {

static const int menuHeight = 140;// The Height fo the context menu.
static const int menuPadding = 60;// This is padding for deciding when to modify context menu position.
static const int spacePadding = 0; // 24;// This is for making context menu closer to the handles.
static const int textSelectionScrollSize = 50;// Scroll step when selection handler is moving out of viewport.

static bool IsRectEmpty(const gfx::Rect& rect) {
  if (rect.x() == 0 && rect.y() == 0 && rect.height() == 0 && rect.width() == 0) {
    LOG(INFO) << "SelectionControllerEfl:IsRectEmpty : true";
    return true;
  }
  return false;
}

SelectionControllerEfl::SelectionControllerEfl(EWebView* parent_view)
    :  parent_view_(parent_view),
       mouse_press_(false),
       scrolling_(false),
       long_mouse_press_(false),
       selection_data_(new SelectionBoxEfl(parent_view)),
       start_handle_(new SelectionHandleEfl(this, SelectionHandleEfl::HANDLE_TYPE_LEFT, parent_view->evas_object())),
       end_handle_(new SelectionHandleEfl(this, SelectionHandleEfl::HANDLE_TYPE_RIGHT, parent_view->evas_object())),
       input_handle_(new SelectionHandleEfl(this, SelectionHandleEfl::HANDLE_TYPE_INPUT, parent_view->evas_object())),
       magnifier_(new SelectionMagnifierEfl(this)) {
  evas_object_event_callback_add(parent_view_->evas_object(), EVAS_CALLBACK_MOVE, &EvasParentViewMoveCallback, this);
}

void SelectionControllerEfl::SetSelectionStatus(bool enable) {
  LOG(INFO) << "SelectionControllerEfl::SetSelectionStatus " << enable;
  selection_data_->SetStatus(enable);
}

bool SelectionControllerEfl::GetSelectionStatus() const {
  LOG(INFO) << "SelectionControllerEfl::GetSelectionStatus " << selection_data_->GetStatus();
  return selection_data_->GetStatus();
}

void SelectionControllerEfl::SetSelectionEditable(bool enable) {
  LOG(INFO) << "SelectionControllerEfl::SetSelectionEditable" << enable;
  selection_data_->SetEditable(enable);
}

bool SelectionControllerEfl::GetSelectionEditable() const {
  LOG(INFO) << "SelectionControllerEfl::GetSelectionEditable " << selection_data_->GetEditable();
  return selection_data_->GetEditable();
}

void SelectionControllerEfl::SetCaretSelectionStatus(const bool enable) {
  LOG(INFO) << "SelectionControllerEfl::SetCaretSelectionStatus : " << enable;
  selection_data_->SetCaretSelectionStatus(enable);
}

bool SelectionControllerEfl::GetCaretSelectionStatus() const {
  LOG(INFO) << "SelectionControllerEfl::GetCaretSelectionStatus : " << selection_data_->GetCaretSelectionStatus();
  return selection_data_->GetCaretSelectionStatus();
}

void SelectionControllerEfl::SetScrollStatus(const bool enable) {
  LOG(INFO) << __PRETTY_FUNCTION__ << " enable : " << enable;
  scrolling_ = enable;
  if (enable)
    Clear();
  else
    ShowHandleAndContextMenuIfRequired();
}

bool SelectionControllerEfl::GetScrollStatus() {
  LOG(INFO) << __PRETTY_FUNCTION__ << " " << scrolling_;
  return scrolling_;
}

void SelectionControllerEfl::UpdateSelectionData(const base::string16& text) {
  selection_data_->UpdateSelectStringData(text);
}

void SelectionControllerEfl::UpdateMagnifierScreen(Evas_Object* img) {
  magnifier_->UpdateScreen(img);
}

void SelectionControllerEfl::UpdateSelectionDataAndShow(const gfx::Rect& left_rect, const gfx::Rect& right_rect, bool is_anchor_first) {
  LOG(INFO) << __PRETTY_FUNCTION__;
  selection_data_->UpdateRectData(left_rect, right_rect, is_anchor_first);

  if (!IsSelectionValid(left_rect, right_rect)) {
    selection_data_->ClearRectData();
    Clear();
    return;
  }

  // Do not show the context menu and handlers untill long mouse press is released
  if (long_mouse_press_)
    return;

  // Do not show the context menu and handlers while page is scrolling
  if (scrolling_)
    return;

  ShowHandleAndContextMenuIfRequired();
}

void SelectionControllerEfl::ShowHandleAndContextMenuIfRequired() {
  LOG(INFO) << __PRETTY_FUNCTION__;
  if (!selection_data_->GetStatus())
    return;

  Clear();

  // Is in edit field and no text is selected. show only single handle
  if (selection_data_->IsInEditField() && GetCaretSelectionStatus()) {
    gfx::Rect left = selection_data_->GetLeftRect();
    input_handle_->SetBasePosition(gfx::Point(left.x(), left.y()));
    input_handle_->SetCursorHandlerStatus(true);
    input_handle_->Move(gfx::Point(left.x(), left.y() + left.height()));
    input_handle_->Show();

    if (!mouse_press_)
      parent_view_->ShowContextMenu(*(selection_data_->GetContextMenuParams()), MENU_TYPE_SELECTION);

    return;
  }

  gfx::Rect left = selection_data_->GetLeftRect();
  gfx::Rect right = selection_data_->GetRightRect();

  if (left.x() == 0 && left.y() == 0 && right.x() == 0 && right.y() == 0) {
    selection_data_->ClearRectData();
    return;
  }
  // The base position of start_handle should be set to the middle of the left rectangle.
  // Otherwise the start_handle may be shifted up when the right_handle is moving
  start_handle_->SetBasePosition(gfx::Point(left.x(), left.y() + (left.height() / 2)));
  start_handle_->Move(gfx::Point(left.x(), left.y() + left.height()));
  start_handle_->Show();


  // The base position of end_handle should be set to the middle of the right rectangle.
  // Otherwise the end_handle may be shifted up when the left_handle is moving
  end_handle_->SetBasePosition(gfx::Point(right.x(), right.y() + (right.height() / 2)));
  end_handle_->Move(gfx::Point(right.x() + right.width(), right.y() + right.height()));
  end_handle_->Show();

  // Do not show the context menu during selection extend
  if (!mouse_press_)
    parent_view_->ShowContextMenu(*(selection_data_->GetContextMenuParams()), MENU_TYPE_SELECTION);
  parent_view_->QuerySelectionStyle();
}

void SelectionControllerEfl::HideHandle() {
  SetCaretSelectionStatus(false);
  Clear();
}

void SelectionControllerEfl::HideHandleAndContextMenu() {
  parent_view_->CancelContextMenu(0);
  HideHandle();
}

bool SelectionControllerEfl::IsAnyHandleVisible() const {
  return (start_handle_->IsVisible() ||
          end_handle_->IsVisible() ||
          input_handle_->IsVisible());
}

void SelectionControllerEfl::Clear() {
  start_handle_->Hide();
  end_handle_->Hide();
  input_handle_->Hide();
}

void SelectionControllerEfl::OnMouseDown(const gfx::Point& touch_point) {
  // Hide context menu on mouse down
  parent_view_->CancelContextMenu(0);
  mouse_press_ = true;
  magnifier_->UpdateLocation(touch_point);
  magnifier_->Move(touch_point);
  ShowHandleAndContextMenuIfRequired();
}

void SelectionControllerEfl::OnMouseMove(const gfx::Point& touch_point, bool on_curson_handle) {
  // FIXME : Check the text Direction later
  magnifier_->UpdateLocation(touch_point);
  magnifier_->Move(touch_point);
  if (!on_curson_handle)
    parent_view_->SelectRange(start_handle_->GetBasePosition(), end_handle_->GetBasePosition());
  else
    parent_view_->MoveCaret(input_handle_->GetBasePosition());
}

void SelectionControllerEfl::OnMouseUp(const gfx::Point& touch_point) {
  selection_data_->UpdateHandleData();
  mouse_press_ = false;
  magnifier_->Hide();
  parent_view_->ShowContextMenu(*(selection_data_->GetContextMenuParams()), MENU_TYPE_SELECTION);
}

void SelectionControllerEfl::GetSelectionBounds(gfx::Rect* left, gfx::Rect* right) {
  if (left)
    *left = selection_data_->GetLeftRect();
  if (right)
    *right = selection_data_->GetRightRect();
}

void SelectionControllerEfl::HandleLongPressEvent(const gfx::Point& touch_point) {
  long_mouse_press_ = true;
  magnifier_->HandleLongPress(touch_point);
}

void SelectionControllerEfl::HandleLongPressMoveEvent(const gfx::Point& touch_point) {
  parent_view_->SelectClosestWord(touch_point);

  if (selection_data_->GetCaretSelectionStatus()) {
    parent_view_->MoveCaret(touch_point);
    SetSelectionStatus(true);
    ShowHandleAndContextMenuIfRequired();
  } else{
    parent_view_->SelectClosestWord(touch_point);
  }
}

void SelectionControllerEfl::HandleLongPressEndEvent() {
  long_mouse_press_ = false;
  if (selection_data_->GetCaretSelectionStatus()) {
    SetSelectionStatus(true);
    SetSelectionEditable(true);
  }
  ShowHandleAndContextMenuIfRequired();
}

bool SelectionControllerEfl::IsSelectionValid(const gfx::Rect& left_rect, const gfx::Rect& right_rect) {
  LOG(INFO) << __PRETTY_FUNCTION__ << " l_x = " << left_rect.x() << " l_y = " << left_rect.y() << " r_x = " << right_rect.x() << " r_y = " << right_rect.y();
  // For all normal cases the widht will be 0 and we want to check empty which Implies
  // x, y, h w all to be 0
  if ((IsRectEmpty(left_rect) || IsRectEmpty(right_rect))) {
    SetSelectionStatus(false);
    return false;
  }

  // The most of sites return width of each rects as 0 when text is selected.
  // However, some websites that have viewport attributes on meta tag v
  // return width 0 right after they are loaded, even though text is not selected.
  // Thus the width is not sufficient for checking selection condition.
  // Further invesitigation showed left_rect and right_rect always have the same x,y values
  // for such cases. So, the equality for x and y rather than width should be tested.
  if (left_rect.x()==right_rect.x() && left_rect.y()==right_rect.y() &&
      !selection_data_->IsInEditField() && !mouse_press_) {
    SetSelectionStatus(false);
    return false;
  }

  LOG(INFO) << "SelectionControllerEfl::IsSelectionValid true";
  if (!GetSelectionStatus())
    SetSelectionStatus(true);

  if (left_rect.x() != right_rect.x() || left_rect.y() != right_rect.y() &&
      selection_data_->IsInEditField() && GetCaretSelectionStatus()) {
    if (!long_mouse_press_)
      SetCaretSelectionStatus(false);
  }

  LOG(INFO) << "SelectionControllerEfl::IsSelectionValid true";
  return true;
}

void SelectionControllerEfl::ClearSelection() {
  LOG(INFO) << __PRETTY_FUNCTION__;
  Clear();
  selection_data_->SetStatus(false);
  SetSelectionEditable(false);
  SetCaretSelectionStatus(false);
}

void SelectionControllerEfl::OnParentParentViewMove() {
  LOG(INFO) << __PRETTY_FUNCTION__;
  parent_view_->CancelContextMenu(0);
  start_handle_->Move(start_handle_->GetBasePosition());
  end_handle_->Move(end_handle_->GetBasePosition());
}

gfx::Rect SelectionControllerEfl::GetLeftRect() {
  return selection_data_->GetLeftRect();
}

gfx::Rect SelectionControllerEfl::GetRightRect() {
  return selection_data_->GetRightRect();
}

void SelectionControllerEfl::ChangeContextMenuPosition(gfx::Point& position, int& drawDirection) {
  drawDirection = DirectionNone; // Giving default Direction.
  int handleHeight, webViewX, webViewY, webViewWidth, webViewHeight;
  gfx::Rect imeRect;
  edje_object_part_geometry_get(input_handle_->evas_object(), "handle", 0, 0, 0, &handleHeight);
  evas_object_geometry_get(GetParentView()->evas_object(), &webViewX, &webViewY, &webViewWidth, &webViewHeight);
  gfx::Rect viewportRect = gfx::Rect(webViewX, webViewY, webViewWidth, webViewHeight);

  if (GetParentView()->IsIMEShow()) { // Get the Visible Rect .
    imeRect = GetParentView()->GetIMERect();
    if ((viewportRect.y() + viewportRect.height()) > imeRect.y())
        viewportRect.set_height(imeRect.y() - viewportRect.y());
  }

  gfx::Rect leftHandleRect = selection_data_->GetLeftRect();
  leftHandleRect.set_x(webViewX + leftHandleRect.x());
  leftHandleRect.set_y(webViewY + leftHandleRect.y());

  if (!GetCaretSelectionStatus()) {
    gfx::Rect rightHandleRect = selection_data_->GetRightRect();
    rightHandleRect.set_x(webViewX + rightHandleRect.x());
    rightHandleRect.set_y(webViewY + rightHandleRect.y());
    gfx::Rect oldLeftHandleRect = leftHandleRect;
    gfx::Rect oldRightHandleRect = rightHandleRect;
    gfx::Rect effectiveRect;
    bool isEffecrtiveRectAvailable = false;

    bool isTop = false;
    bool isRightHandle = false;
    bool doConsiderRightHandle = false;
    bool doNotConsiderMenuUpward = false;

    //if (leftHandleRect.x() < 0)
    //    leftHandleRect.set_x(0);
    //if (leftHandleRect.y() < 0)
    //    leftHandleRect.set_y(0);

    // Giving first preference to left handle.
    if (leftHandleRect.IsEmpty() && viewportRect.Contains(leftHandleRect.x(), leftHandleRect.y())) {
      isTop = start_handle_->IsTop();
      effectiveRect = leftHandleRect;
      isEffecrtiveRectAvailable = true;
      // Check whether Menu will overlap the right handle or not.
      if (leftHandleRect != rightHandleRect) {
        if (!isTop) {
          // If there is sufficient space above the handler that Menu can be placed. then shift context menu
          // then no need to change effectiveRect from leftHandleRect to rightHandleRect.
          bool directionUp = effectiveRect.y() - menuHeight > viewportRect.y() && (effectiveRect.y() - (viewportRect.y() + menuHeight) > menuPadding);
          if (!directionUp && ((leftHandleRect.y() + leftHandleRect.height()) + menuHeight) > rightHandleRect.y()) {
            doConsiderRightHandle = true;
            doNotConsiderMenuUpward = true; // As if we draw the direction is UP it will overlap with left Handle.
          }
        }
      }
    } else {
      doConsiderRightHandle = true;
    }

    if (doConsiderRightHandle && rightHandleRect.IsEmpty() && viewportRect.Contains(rightHandleRect.x(), rightHandleRect.y())) {
      effectiveRect = rightHandleRect;
      isEffecrtiveRectAvailable = true;
      isTop = end_handle_->IsTop();
      isRightHandle = true;
    }

    if (isEffecrtiveRectAvailable) {
      //We will go for UpWard, DownWard, Left, Right directions.
      if (effectiveRect.y() - menuHeight > viewportRect.y() && (effectiveRect.y() - (viewportRect.y() + menuHeight) > menuPadding) && !doNotConsiderMenuUpward) {
        if (isTop) {
          if (isRightHandle) {
            position.set_y(effectiveRect.y() - spacePadding - handleHeight);
          } else {
            position.set_y(effectiveRect.y() - spacePadding - handleHeight);
            //position.set_y(position.y() - effectiveRect.height());
          }
        } else {
          position.set_y(effectiveRect.y() - spacePadding);
        }
        drawDirection = DirectionUp;
      } else if ((effectiveRect.y() + effectiveRect.height()) + menuHeight < (viewportRect.y() + viewportRect.height())) {
        position.set_y((effectiveRect.y() + effectiveRect.height()) - spacePadding + handleHeight);
        drawDirection = DirectionDown;
      } else if (effectiveRect.x() < (viewportRect.x() + viewportRect.width()/2)) {
        position.set_x((effectiveRect.x() + effectiveRect.width()) + spacePadding + handleHeight);
        position.set_y(effectiveRect.CenterPoint().y());
        drawDirection = DirectionLeft;
      } else {
        position.set_x(effectiveRect.x());
        position.set_y(effectiveRect.CenterPoint().y());
        drawDirection = DirectionRight;
      }
    } else {
      if (oldLeftHandleRect.y() < viewportRect.y() && oldRightHandleRect.y() > (viewportRect.y() + viewportRect.height()) && viewportRect.height() <= webViewHeight) {
        position.set_y(viewportRect.CenterPoint().y() - spacePadding);
      } else {
        position.set_y(position.y() + spacePadding + handleHeight);// This value is chosen based on the how much close the context menu arrow should come to word.
      }
      drawDirection = DirectionNone;
    }

    return;
  }

  if (!selection_data_->IsInEditField())
    return;

  position.set_y(leftHandleRect.y() - spacePadding);

  if (input_handle_->IsTop()) {
    position.set_y(position.y() - leftHandleRect.height() - handleHeight);
    return;
  }

  // The Width of the context menu is menuHeight so comapairing current position with the viewport point plus menuHeight.
  // If position is less than this value then set new position for the contextmenu.
  if ((position.y() <= (webViewY + menuHeight)) || ((position.y() - (webViewY + menuHeight)) <= menuPadding)) {
    if (leftHandleRect.IsEmpty()) {
      position.set_y(position.y() + spacePadding + handleHeight);// This value is chosen based on the how much close the context menu arrow should come to word.
      drawDirection = DirectionDown;
     return;
    }

    position.set_y((leftHandleRect.y() + leftHandleRect.height()) - spacePadding + handleHeight);
    drawDirection = DirectionDown;
    return;
  }

  return;
}

}
