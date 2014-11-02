// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DISAMBIGUATION_POPUP_EFL_H
#define DISAMBIGUATION_POPUP_EFL_H

#include "ui/gfx/point.h"
#include "ui/gfx/rect.h"

#include <Evas.h>

class SkBitmap;

namespace content {

class RenderWidgetHostViewEfl;

class DisambiguationPopupEfl {
 public:
  DisambiguationPopupEfl(Evas_Object* parent_view, RenderWidgetHostViewEfl*);
  ~DisambiguationPopupEfl();

  void Show(const gfx::Rect& target, const SkBitmap& display_image);
  void Dismiss();

  bool IsVisible();

  void SetImageSize(int&, int&, int, int, const gfx::Point&);
  void UpdateImage(const SkBitmap& display_image);
  void Clear();

  void HandleMouseEventUp(Evas_Event_Mouse_Up* up_event);
  void HandleMouseEventDown(Evas_Event_Mouse_Down* down_event);

 private:
  template<class T>
  void HandleMouseEvent(T* event);

  static void Dismissed(void*, Evas_Object*, void*);

  gfx::Rect target_rect_;

  RenderWidgetHostViewEfl* rwhv_;
  Evas_Object* parent_view_;
  // Image displayed on popup
  Evas_Object* content_image_;
  Evas_Object* popup_;

  int height_;
  int width_;

  DISALLOW_COPY_AND_ASSIGN(DisambiguationPopupEfl);
};

} // namespace content

#endif
