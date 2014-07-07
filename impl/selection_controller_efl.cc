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
#include "selection_box_efl.h"
#include "selection_magnifier_efl.h"
#include "ui/gfx/screen.h"

#include <Edje.h>

// FIXME: To use BLINK_TEXT_DIR, following line needs to be modified later.
#define WEBKIT_TEXT_DIR "/usr/share/ewebkit2-0/locale/po"

namespace content {

static const int menuHeight = 140;// The Height fo the context menu.
static const int menuPadding = 60;// This is padding for deciding when to modify context menu position.
static const int spacePadding = 0; // 24;// This is for making context menu closer to the handles.
static const int textSelectionScrollSize = 50;// Scroll step when selection handler is moving out of viewport.

static bool IsRectEmpty(const gfx::Rect& rect) {
  return (rect.x() == 0 && rect.y() == 0 && rect.height() == 0 && rect.width() == 0);
}

SelectionControllerEfl::SelectionControllerEfl(EWebView* parent_view)
    :  parent_view_(parent_view),
       mouse_press_(false),
       scrolling_(false),
       expecting_update_(false),
       long_mouse_press_(false),
       selection_data_(new SelectionBoxEfl(parent_view)),
       start_handle_(new SelectionHandleEfl(*this, SelectionHandleEfl::HANDLE_TYPE_LEFT, parent_view->evas_object())),
       end_handle_(new SelectionHandleEfl(*this, SelectionHandleEfl::HANDLE_TYPE_RIGHT, parent_view->evas_object())),
       input_handle_(new SelectionHandleEfl(*this, SelectionHandleEfl::HANDLE_TYPE_INPUT, parent_view->evas_object())),

       magnifier_(new SelectionMagnifierEfl(this)) {
  evas_object_event_callback_add(parent_view_->evas_object(), EVAS_CALLBACK_MOVE, &EvasParentViewMoveCallback, this);

#if defined(OS_TIZEN)
  setlocale(LC_ALL, vconf_get_str(VCONFKEY_LANGSET));
  bindtextdomain("WebKit", WEBKIT_TEXT_DIR);
  vconf_notify_key_changed(VCONFKEY_LANGSET, PlatformLanguageChanged, this);
#endif
}

void SelectionControllerEfl::SetSelectionStatus(bool enable) {
  TRACE_EVENT1("selection,efl", __PRETTY_FUNCTION__, "enable", enable);
  selection_data_->SetStatus(enable);
}

bool SelectionControllerEfl::GetSelectionStatus() const {
  TRACE_EVENT1("selection,efl", __PRETTY_FUNCTION__,
               "status", selection_data_->GetStatus());
  return selection_data_->GetStatus();
}

#if defined(OS_TIZEN)
void SelectionControllerEfl::PlatformLanguageChanged(keynode_t* keynode, void* data) {
  setlocale(LC_ALL, vconf_get_str(VCONFKEY_LANGSET));
  bindtextdomain("WebKit", WEBKIT_TEXT_DIR);

  SelectionControllerEfl* selection_controller = static_cast<SelectionControllerEfl*>(data);
  if (!selection_controller)
    return;

  if (selection_controller->GetSelectionStatus())
    selection_controller->ClearSelectionViaEWebView();
  selection_controller->HideHandleAndContextMenu();
}
#endif

void SelectionControllerEfl::SetSelectionEditable(bool enable) {
  TRACE_EVENT1("selection,efl", __PRETTY_FUNCTION__, "enable", enable);
  selection_data_->SetEditable(enable);
}

bool SelectionControllerEfl::GetSelectionEditable() const {
  TRACE_EVENT1("selection,efl", __PRETTY_FUNCTION__,
               "editable", selection_data_->GetEditable());
  return selection_data_->GetEditable();
}

void SelectionControllerEfl::SetCaretSelectionStatus(const bool enable) {
  TRACE_EVENT1("selection,efl", __PRETTY_FUNCTION__, "caret selection", enable);
  selection_data_->SetCaretSelectionStatus(enable);
}

bool SelectionControllerEfl::GetCaretSelectionStatus() const {
  TRACE_EVENT1("selection,efl", __PRETTY_FUNCTION__,
               "caret selection", selection_data_->GetCaretSelectionStatus());
  return selection_data_->GetCaretSelectionStatus();
}

void SelectionControllerEfl::SetScrollStatus(const bool enable) {
  TRACE_EVENT1("selection,efl", __PRETTY_FUNCTION__, "scroll status", enable);
  scrolling_ = enable;
  if (enable)
    Clear();
  else
    ShowHandleAndContextMenuIfRequired();
}

bool SelectionControllerEfl::GetScrollStatus() {
  TRACE_EVENT1("selection,efl", __PRETTY_FUNCTION__, "scroll status", scrolling_);
  return scrolling_;
}

void SelectionControllerEfl::UpdateSelectionData(const base::string16& text) {
  selection_data_->UpdateSelectStringData(text);
}

void SelectionControllerEfl::UpdateMagnifierScreen(const SkBitmap& display_image) {
  magnifier_->UpdateScreen(display_image);
}

void SelectionControllerEfl::ClearSelectionViaEWebView() {
  parent_view_->ClearSelection();
}

void SelectionControllerEfl::UpdateSelectionDataAndShow(const gfx::Rect& left_rect, const gfx::Rect& right_rect, bool) {
  TRACE_EVENT0("selection,efl", __PRETTY_FUNCTION__);
  selection_data_->UpdateRectData(left_rect, right_rect);

  if (!IsSelectionValid(left_rect, right_rect)) {
    selection_data_->ClearRectData();
    Clear();
  } else {
    if (selection_data_->GetEditable()) {
      // In case we're selecting text in editable text field we've already sent swapped
      // coordinates from OnMouseMove. No need to do it for the second time.
      ShowHandleAndContextMenuIfRequired();
    } else {
      ShowHandleAndContextMenuIfRequired(left_rect < right_rect);
    }
  }
  expecting_update_ = false;
}

void SelectionControllerEfl::ShowHandleAndContextMenuIfRequired(bool anchor_first) {
  TRACE_EVENT0("selection,efl", __PRETTY_FUNCTION__);
  if (!selection_data_->GetStatus())
    return;

  if (scrolling_)
    return;

  float dpi_scale;
  gfx::Rect left, right;
  if (anchor_first) {
    left = selection_data_->GetLeftRect();
    right = selection_data_->GetRightRect();
  } else {
    right = selection_data_->GetLeftRect();
    left = selection_data_->GetRightRect();
  }

  dpi_scale = gfx::Screen::GetNativeScreen()->GetPrimaryDisplay().device_scale_factor();
  int hitX, hitY;
  hitX = selection_data_->GetContextMenuParams()->x;
  hitY = selection_data_->GetContextMenuParams()->y;

  if (dpi_scale) {
    hitX = hitX / dpi_scale;
    hitY = hitY / dpi_scale;
  }

  scoped_ptr<tizen_webview::Hit_Test> hit_test(
    parent_view_->RequestHitTestDataAt(hitX, hitY, tizen_webview::TW_HIT_TEST_MODE_DEFAULT));

  // Is in edit field and no text is selected. show only single handle
  if (selection_data_->IsInEditField() && left == right) {

    if (hit_test.get() &&
        selection_data_->IsInEditField() &&
        !(hit_test->GetResultContext() & tizen_webview::TW_HIT_TEST_RESULT_CONTEXT_EDITABLE))
      return;

    gfx::Rect left = selection_data_->GetLeftRect();
    input_handle_->SetBasePosition(gfx::Point(left.x(), left.y()));
    input_handle_->Move(left.bottom_right());
    input_handle_->Show();
    start_handle_->Hide();
    end_handle_->Hide();

    if (!mouse_press_)
      parent_view_->ShowContextMenu(*(selection_data_->GetContextMenuParams()), MENU_TYPE_SELECTION);
    parent_view_->QuerySelectionStyle();
    return;
  } else {
    input_handle_->Hide();
  }

  if (left.x() == 0 && left.y() == 0 && right.x() == 0 && right.y() == 0) {
    selection_data_->ClearRectData();
    return;
  }

  if (hit_test.get() &&
    !(hit_test->GetResultContext() & tizen_webview::TW_HIT_TEST_RESULT_CONTEXT_SELECTION))
    return;

  // The base position of start_handle should be set to the middle of the left rectangle.
  // Otherwise the start_handle may be shifted up when the right_handle is moving
  start_handle_->SetBasePosition(left.CenterPoint());
  start_handle_->Move(left.bottom_left());
  if (left.x() >= visibility_rect_.x() && left.x() <= visibility_rect_.right())
    start_handle_->Show();
  else
    start_handle_->Hide();

  // The base position of end_handle should be set to the middle of the right rectangle.
  // Otherwise the end_handle may be shifted up when the left_handle is moving
  end_handle_->SetBasePosition(right.CenterPoint());
  end_handle_->Move(right.bottom_right());
  if (right.x() >= visibility_rect_.x() && right.x() <= visibility_rect_.right())
    end_handle_->Show();
  else
    end_handle_->Hide();

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

bool SelectionControllerEfl::IsShowingMagnifier() {
  if(magnifier_->IsShowing())
    return true;
  return false;
}

void SelectionControllerEfl::OnMouseDown(const gfx::Point& touch_point) {
  // Hide context menu on mouse down
  parent_view_->CancelContextMenu(0);
  mouse_press_ = true;
  magnifier_->UpdateLocation(touch_point);
  magnifier_->Move(touch_point);
  ShowHandleAndContextMenuIfRequired();
}

void SelectionControllerEfl::OnMouseMove(const gfx::Point& touch_point, SelectionHandleEfl::HandleType handle) {
  // FIXME : Check the text Direction later
  magnifier_->UpdateLocation(touch_point);
  magnifier_->Move(touch_point);
  expecting_update_ = true;
  switch (handle) {
    case SelectionHandleEfl::HANDLE_TYPE_INPUT:
      parent_view_->MoveCaret(input_handle_->GetBasePosition());
      return;
    case SelectionHandleEfl::HANDLE_TYPE_LEFT:
      if (GetSelectionEditable()) {
        // Form elements only support scrolling of extent/end caret. To
        // move the start element we need to swap the coordinates provided
        // to SelectRange function.
        parent_view_->SelectRange(end_handle_->GetBasePosition(),
                                  start_handle_->GetBasePosition());
        return;
      }
    case SelectionHandleEfl::HANDLE_TYPE_RIGHT:
      parent_view_->SelectRange(start_handle_->GetBasePosition(),
                                end_handle_->GetBasePosition());
      return;
  }
}

void SelectionControllerEfl::OnMouseUp(const gfx::Point& touch_point) {
  mouse_press_ = false;
  magnifier_->Hide();
  start_handle_->SetBasePosition(selection_data_->GetLeftRect().bottom_left());
  end_handle_->SetBasePosition(selection_data_->GetRightRect().bottom_right());
  ShowHandleAndContextMenuIfRequired();
}

void SelectionControllerEfl::GetSelectionBounds(gfx::Rect* left, gfx::Rect* right) {
  if (left)
    *left = selection_data_->GetLeftRect();
  if (right)
    *right = selection_data_->GetRightRect();
}

void SelectionControllerEfl::HandleLongPressEvent(const gfx::Point& touch_point) {
  long_mouse_press_ = true;
  Clear();
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
  TRACE_EVENT2("selection,efl", __PRETTY_FUNCTION__,
               "left_rect", left_rect.ToString(),
               "right_rect", right_rect.ToString());
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
  if (left_rect.x() == right_rect.x() && left_rect.y() == right_rect.y() &&
      !selection_data_->IsInEditField()  && !mouse_press_ && !expecting_update_) {
    SetSelectionStatus(false);
    return false;
  }

  if (!GetSelectionStatus())
    SetSelectionStatus(true);

  if (left_rect.x() != right_rect.x() || left_rect.y() != right_rect.y() &&
      selection_data_->IsInEditField() && GetCaretSelectionStatus()) {
    if (!long_mouse_press_)
      SetCaretSelectionStatus(false);
  }

  return true;
}

void SelectionControllerEfl::ClearSelection() {
  TRACE_EVENT0("selection,efl", __PRETTY_FUNCTION__);
  Clear();
  selection_data_->SetStatus(false);
  SetSelectionEditable(false);
  SetCaretSelectionStatus(false);
}

void SelectionControllerEfl::OnParentParentViewMove() {
  TRACE_EVENT0("selection,efl", __PRETTY_FUNCTION__);
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

Eina_Bool SelectionControllerEfl::TextSelectionDown(int x, int y) {
  /*
   * According to webkit-efl textSelectionDown is used on long press gesture, we already
   * have implementation for handling this gesture in SelectionControllerEfl so we just
   * fallback into it. Although I'm not totally sure that this is expected behaviour as there
   * is no clear explanation what should this API do.
   *
   * Reference from webkit-efl:
   * Source/WebKit2/UIProcess/API/efl/ewk_view.cpp line 614
   */
  if (!long_mouse_press_) {
    HandleLongPressEvent(gfx::Point(x, y));
    return EINA_TRUE;
  }

  return EINA_FALSE;
}

Eina_Bool SelectionControllerEfl::TextSelectionUp(int /*x*/, int /*y*/) {
  /*
   * According to webkit-efl textSelectionUp is used when MouseUp event occurs. We already
   * have implementation for handling MouseUp after long press in SelectionMagnifierEfl so we just
   * fallback into it. Although I'm not totally sure that this is expected behaviour as there
   * is no clear explanation what should this API do.
   *
   * Reference from webkit-efl:
   * Source/WebKit2/UIProcess/API/efl/tizen/TextSelection.cpp line 807
   */

  if (long_mouse_press_) {
    magnifier_->OnAnimatorUp();
    return EINA_TRUE;
  }

  return EINA_FALSE;
}

}
