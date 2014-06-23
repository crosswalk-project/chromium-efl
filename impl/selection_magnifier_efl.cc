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

#include "base/path_service.h"
#include "base/files/file_path.h"
#include "eweb_view.h"
#include "paths_efl.h"
#include "selection_magnifier_efl.h"
#include "selection_controller_efl.h"

#include <Elementary.h>

namespace content {

// Inherited from WK2 w.r.t tizen style
const int kWidthOffset = 32;
const int kHeightOffset = 60;
const float kZoomScale = 0.66;

SelectionMagnifierEfl::SelectionMagnifierEfl(content::SelectionControllerEfl* controller)
  : controller_(controller),
    content_image_(0),
    animator_(0),
    shown_(false) {
  Evas_Object* top_widget = elm_object_top_widget_get(
      elm_object_parent_widget_get(controller->GetParentView()->evas_object()));
  if (!top_widget)
    top_widget = controller->GetParentView()->evas_object();

  base::FilePath edj_dir;
  base::FilePath magnifier_edj;
  PathService::Get(PathsEfl::EDJE_RESOURCE_DIR, &edj_dir);
  magnifier_edj = edj_dir.Append(FILE_PATH_LITERAL("Magnifier.edj"));

  container_ = elm_layout_add(top_widget);
  elm_layout_file_set(container_, magnifier_edj.AsUTF8Unsafe().c_str(), "magnifier");
  int get_height, get_width;
  edje_object_part_geometry_get(elm_layout_edje_get(container_), "bg", 0, 0, &get_width, &get_height);
  width_ = (int)((get_width - kWidthOffset) * kZoomScale);
  height_ = (int)((get_height - kHeightOffset) * kZoomScale);
}

SelectionMagnifierEfl::~SelectionMagnifierEfl() {
  if (content_image_) {
    evas_object_del(content_image_);
    content_image_ = 0;
  }
  if (container_)
    evas_object_del(container_);
}

void SelectionMagnifierEfl::HandleLongPress(const gfx::Point& touch_point) {
  evas_object_event_callback_add(controller_->GetParentView()->evas_object(),
                                 EVAS_CALLBACK_MOUSE_UP,
                                 OnAnimatorUp,
                                 this);
  UpdateLocation(touch_point);
  Move(touch_point);
  Show();
  animator_ = ecore_animator_add(&SelectionMagnifierEfl::MoveAnimatorCallback, this);
}

Eina_Bool SelectionMagnifierEfl::MoveAnimatorCallback(void* data) {
  SelectionMagnifierEfl* magnifier_data = static_cast<SelectionMagnifierEfl*>(data);

  Evas_Coord_Point point;
  evas_pointer_canvas_xy_get(evas_object_evas_get(magnifier_data->controller_->GetParentView()->evas_object()),
                             &point.x,
                             &point.y);
  gfx::Point display_point(point.x, point.y);
  magnifier_data->controller_->HandleLongPressMoveEvent(display_point);
  magnifier_data->UpdateLocation(display_point);
  magnifier_data->Move(display_point);
  return ECORE_CALLBACK_RENEW;
}

void SelectionMagnifierEfl::OnAnimatorUp(void* data, Evas*, Evas_Object*, void*) {
  SelectionMagnifierEfl* magnifier_data = static_cast<SelectionMagnifierEfl*>(data);
  DCHECK(magnifier_data);

  if (magnifier_data)
    magnifier_data->OnAnimatorUp();
}

void SelectionMagnifierEfl::OnAnimatorUp() {
  if (animator_) {
    ecore_animator_del(animator_);
    animator_ = 0;
  }

  evas_object_event_callback_del(controller_->GetParentView()->evas_object(),
                                 EVAS_CALLBACK_MOUSE_UP,
                                 OnAnimatorUp);
  Hide();
  controller_->HandleLongPressEndEvent();
}

void SelectionMagnifierEfl::UpdateLocation(const gfx::Point& location) {
  gfx::Rect content_rect(location.x() - width_/2, location.y() - height_/2, width_, height_);
  int device_x, device_y, device_width, device_height;
  evas_object_geometry_get(controller_->GetParentView()->evas_object(),
                           &device_x,
                           &device_y,
                           &device_width,
                           &device_height);

  // Adjustments for boundry conditions
  if (content_rect.x() < 0)
    content_rect.set_x(0);
  else if ((content_rect.x() + width_) > device_width)
    content_rect.set_x(device_width - width_);

  if (content_rect.y() < 0)
    content_rect.set_y(0);
  else if ((content_rect.y() + height_) > device_height)
    content_rect.set_y(device_height - height_);

  controller_->GetParentView()->GetSnapShotForRect(content_rect);
}

void SelectionMagnifierEfl::UpdateScreen(Evas_Object* img) {
  if (content_image_) {
    evas_object_del(content_image_);
    content_image_ = 0;
  }

  //Evas* evas = evas_object_evas_get(controller_->GetParentView()->evas_object());
  content_image_ = img; //evas_object_image_filled_add(evas);

  //evas_object_image_colorspace_set(content_image_, EVAS_COLORSPACE_ARGB8888);
  // evas_object_image_size_set(content_image_, width_, height_);
  // evas_object_image_alpha_set(content_image_, EINA_TRUE);

  // evas_object_image_data_copy_set(content_image_, display_image.getPixels());
  evas_object_size_hint_min_set(content_image_, width_, height_);
  // evas_object_resize(content_image_, width_, height_);
  // evas_object_image_filled_set(content_image_, true);
  evas_object_show(content_image_);

  elm_object_part_content_set(container_, "swallow", content_image_);
  evas_object_pass_events_set(content_image_, EINA_TRUE);
  evas_object_clip_set(content_image_, container_);

  evas_object_layer_set(container_, EVAS_LAYER_MAX);
  evas_object_layer_set(content_image_, EVAS_LAYER_MAX);
}

// Inherited from WK2 w.r.t tizen style
const int kMagnifierYOffset = 220;

void SelectionMagnifierEfl::Move(const gfx::Point& location) {
  gfx::Point magnifier_location(location.x(), location.y() - kMagnifierYOffset);
  int device_x, device_y, device_width;
  evas_object_geometry_get(controller_->GetParentView()->evas_object(),
                           &device_x,
                           &device_y,
                           &device_width,
                           0);

  if ((magnifier_location.x() - (width_/2)) <= device_x)
    magnifier_location.set_x(width_/2);
  else if ((magnifier_location.x() + width_/2) > device_width)
    magnifier_location.set_x(device_width - width_/2);

  // Only top condition is considered as for the bottom part the magnifer
  // will always be on top of the touch point so no need to consider
  if (magnifier_location.y() < device_y)
    magnifier_location.set_y(device_y);
  evas_object_move(container_, magnifier_location.x(), magnifier_location.y());
}

void SelectionMagnifierEfl::Show() {
  shown_ = true;
  evas_object_show(container_);
  controller_->GetParentView()->SmartCallback<EWebViewCallbacks::MagnifierShow>().call();
  controller_->GetParentView()->set_magnifier(true);
}

void SelectionMagnifierEfl::Hide() {
  shown_ = false;
  evas_object_hide(content_image_);
  evas_object_hide(container_);
  controller_->GetParentView()->SmartCallback<EWebViewCallbacks::MagnifierHide>().call();
  controller_->GetParentView()->set_magnifier(false);
}

}
