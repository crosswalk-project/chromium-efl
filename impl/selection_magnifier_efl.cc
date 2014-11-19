// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/path_service.h"
#include "base/files/file_path.h"
#include "eweb_view.h"
#include "paths_efl.h"
#include "selection_magnifier_efl.h"
#include "selection_controller_efl.h"

#include <Elementary.h>

namespace content {

// Inherited from WK2 w.r.t tizen style
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
  edje_object_part_geometry_get(elm_layout_edje_get(container_), "bg", 0, 0, &width_, &height_);
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
  int device_x, device_y, device_width, device_height;
  evas_object_geometry_get(controller_->GetParentView()->evas_object(),
                           &device_x,
                           &device_y,
                           &device_width,
                           &device_height);

  int zoomedWidth = width_ * kZoomScale;
  int zoomedHeight = height_ * kZoomScale;

  if (zoomedWidth > device_width)
    zoomedWidth = device_width;

  if (zoomedHeight > device_height)
    zoomedHeight = device_height;

  gfx::Rect content_rect(location.x() - zoomedWidth/2 - device_x,
      location.y() - zoomedHeight/2 - device_y,  zoomedWidth, zoomedHeight);

  // Adjustments for boundry conditions
  if (content_rect.x() < 0)
    content_rect.set_x(0);
  else if ((content_rect.x() + zoomedWidth) > device_width)
    content_rect.set_x(device_width - zoomedWidth);

  if (content_rect.y() < 0) // Do not let location to be less then magnifier y value
    content_rect.set_y(0);
  else if (content_rect.y() > device_height - zoomedHeight)
    content_rect.set_y(device_height - zoomedHeight);

  controller_->GetParentView()->GetSnapShotForRect(content_rect);
}

void SelectionMagnifierEfl::UpdateScreen(const SkBitmap& display_image) {
  if (content_image_) {
    evas_object_del(content_image_);
    content_image_ = 0;
  }

  Evas* evas = evas_object_evas_get(controller_->GetParentView()->evas_object());
  content_image_ = evas_object_image_filled_add(evas);
  evas_object_image_size_set(content_image_, width_ * kZoomScale, height_ * kZoomScale);
  evas_object_image_data_copy_set(content_image_, display_image.getPixels());
  evas_object_show(content_image_);

  elm_object_part_content_set(container_, "swallow", content_image_);
  evas_object_pass_events_set(content_image_, EINA_TRUE);
  evas_object_clip_set(content_image_, container_);

  evas_object_layer_set(container_, EVAS_LAYER_MAX);
  evas_object_layer_set(content_image_, EVAS_LAYER_MAX);
}

void SelectionMagnifierEfl::Move(const gfx::Point& location) {
  int device_x, device_y, device_width, device_height;
  evas_object_geometry_get(controller_->GetParentView()->evas_object(),
                           &device_x,
                           &device_y,
                           &device_width,
                           &device_height);

  int magnifier_x = location.x();
  int magnifier_y = location.y() - height_ - kHeightOffset;

  if (magnifier_y < device_y) // Do not let location to be out of screen
    magnifier_y = device_y;
  else if (magnifier_y > device_height + device_y - height_)
    magnifier_y = device_height + device_y - height_;

  if (magnifier_x < device_x + width_/2) // Do not let location to be out of screen
    magnifier_x = device_x + width_/2;
  else if (magnifier_x > device_width + device_x - width_/2)
    magnifier_x = device_width + device_x - width_/2;

  evas_object_move(container_, magnifier_x, magnifier_y);
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
