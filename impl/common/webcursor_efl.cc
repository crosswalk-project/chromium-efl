// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "common/webcursor_efl.h"

#include "base/logging.h"
#include "third_party/WebKit/public/platform/WebCursorInfo.h"

#include <Ecore_X.h>

using namespace blink;

int GetCursorType(int type) {
  switch (type) {
    case WebCursorInfo::TypePointer:
      return ECORE_X_CURSOR_XTERM;
    case WebCursorInfo::TypeCross:
      return ECORE_X_CURSOR_CROSS;
    case WebCursorInfo::TypeHand:
      return ECORE_X_CURSOR_HAND2;
    case WebCursorInfo::TypeIBeam:
      return ECORE_X_CURSOR_XTERM;
    case WebCursorInfo::TypeWait:
      return ECORE_X_CURSOR_WATCH;
    case WebCursorInfo::TypeHelp:
      return ECORE_X_CURSOR_QUESTION_ARROW;
    case WebCursorInfo::TypeEastResize:
      return ECORE_X_CURSOR_RIGHT_SIDE;
    case WebCursorInfo::TypeNorthResize:
      return ECORE_X_CURSOR_TOP_SIDE;
    case WebCursorInfo::TypeNorthEastResize:
      return ECORE_X_CURSOR_TOP_RIGHT_CORNER;
    case WebCursorInfo::TypeNorthWestResize:
      return ECORE_X_CURSOR_TOP_LEFT_CORNER;
    case WebCursorInfo::TypeSouthResize:
      return ECORE_X_CURSOR_BOTTOM_SIDE;
    case WebCursorInfo::TypeSouthEastResize:
      return ECORE_X_CURSOR_BOTTOM_RIGHT_CORNER;
    case WebCursorInfo::TypeSouthWestResize:
      return ECORE_X_CURSOR_BOTTOM_LEFT_CORNER;
    case WebCursorInfo::TypeWestResize:
      return ECORE_X_CURSOR_LEFT_SIDE;
    case WebCursorInfo::TypeNorthSouthResize:
      return ECORE_X_CURSOR_SB_V_DOUBLE_ARROW;
    case WebCursorInfo::TypeEastWestResize:
      return ECORE_X_CURSOR_SB_H_DOUBLE_ARROW;
    case WebCursorInfo::TypeNorthEastSouthWestResize:
    case WebCursorInfo::TypeNorthWestSouthEastResize:
      // There isn't really a useful cursor available for these.
      return ECORE_X_CURSOR_XTERM;
    case WebCursorInfo::TypeColumnResize:
      return ECORE_X_CURSOR_SB_H_DOUBLE_ARROW;  // TODO(evanm): is this correct?
    case WebCursorInfo::TypeRowResize:
      return ECORE_X_CURSOR_SB_V_DOUBLE_ARROW;  // TODO(evanm): is this correct?
    case WebCursorInfo::TypeMiddlePanning:
      return ECORE_X_CURSOR_FLEUR;
    case WebCursorInfo::TypeEastPanning:
      return ECORE_X_CURSOR_SB_RIGHT_ARROW;
    case WebCursorInfo::TypeNorthPanning:
      return ECORE_X_CURSOR_SB_UP_ARROW;
    case WebCursorInfo::TypeNorthEastPanning:
      return ECORE_X_CURSOR_TOP_RIGHT_CORNER;
    case WebCursorInfo::TypeNorthWestPanning:
      return ECORE_X_CURSOR_TOP_LEFT_CORNER;
    case WebCursorInfo::TypeSouthPanning:
      return ECORE_X_CURSOR_SB_DOWN_ARROW;
    case WebCursorInfo::TypeSouthEastPanning:
      return ECORE_X_CURSOR_BOTTOM_RIGHT_CORNER;
    case WebCursorInfo::TypeSouthWestPanning:
      return ECORE_X_CURSOR_BOTTOM_LEFT_CORNER;
    case WebCursorInfo::TypeWestPanning:
      return ECORE_X_CURSOR_SB_LEFT_ARROW;
    case WebCursorInfo::TypeMove:
      return ECORE_X_CURSOR_FLEUR;
    case WebCursorInfo::TypeVerticalText:
      return ECORE_X_CURSOR_XTERM;
    case WebCursorInfo::TypeCell:
      return ECORE_X_CURSOR_XTERM;
    case WebCursorInfo::TypeContextMenu:
      return ECORE_X_CURSOR_XTERM;
    case WebCursorInfo::TypeAlias:
      return ECORE_X_CURSOR_XTERM;
    case WebCursorInfo::TypeProgress:
      return ECORE_X_CURSOR_WATCH;
    case WebCursorInfo::TypeNoDrop:
      return ECORE_X_CURSOR_XTERM;
    case WebCursorInfo::TypeCopy:
      return ECORE_X_CURSOR_XTERM;
    case WebCursorInfo::TypeNone:
      return ECORE_X_CURSOR_XTERM;
    case WebCursorInfo::TypeNotAllowed:
      return ECORE_X_CURSOR_XTERM;
    case WebCursorInfo::TypeZoomIn:
    case WebCursorInfo::TypeZoomOut:
    case WebCursorInfo::TypeGrab:
    case WebCursorInfo::TypeGrabbing:
    case WebCursorInfo::TypeCustom:
      return ECORE_X_CURSOR_XTERM; // Need to check the ECORE_X_CURSOR_XTERM
  }
  NOTREACHED();
  return ECORE_X_CURSOR_XTERM;
}
