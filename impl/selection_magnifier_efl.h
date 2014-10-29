// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
  void UpdateScreen(const SkBitmap& display_image);
  void Move(const gfx::Point& location);
  void Show();
  void Hide();
  bool IsShowing() { return shown_; }
  void OnAnimatorUp();

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

  // Is magnifier showing
  bool shown_;
};

}

#endif
