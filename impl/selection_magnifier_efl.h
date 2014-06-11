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

#ifndef selection_magnifier_efl_h
#define selection_magnifier_efl_h

#include "ui/gfx/image/image_skia.h"
#include "ui/gfx/point.h"

#include <Ecore.h>
#include <Eina.h>
#include <Evas.h>

namespace content {

class SelectionControllerEfl;

class SelectionMagnifierEfl {
 public:
  SelectionMagnifierEfl(content::SelectionControllerEfl* controller);
  ~SelectionMagnifierEfl();

  void HandleLongPress(const gfx::Point& touch_point);
  void UpdateLocation(const gfx::Point& location);
  void UpdateScreen(Evas_Object* img);
  void Move(const gfx::Point& location);
  void Show();
  void Hide();

 private:
  static Eina_Bool MoveAnimatorCallback(void* data);
  static void OnAnimatorUp(void* data, Evas*, Evas_Object*, void*);

  // Parent to send back mouse events
  content::SelectionControllerEfl* controller_;

  // Magnifier
  Evas_Object* container_;

  // Image displayed on popup
  Evas_Object* content_image_;

  // Magnifier Height
  int height_;

  // Magnifier width
  int width_;

  // Handle longmove
  Ecore_Animator* animator_;
};

}

#endif
