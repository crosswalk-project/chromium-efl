// Copyright 2000 - 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "wkext_motion.h"
#include "MotionUI.h"
#include "base/logging.h"

#if !defined(EWK_BRINGUP)
void wkext_motion_tilt_enable_set(Evas_Object* view, int enable, unsigned int sensitivity) {
  LOG(ERROR)<<"wkext_motion_tilt_enable_set";
  MotionUI::motionUI().setTiltToZoom(view, enable, sensitivity);
}

void wkext_motion_tilt_start(Evas_Object* view, const Evas_Coord_Point* position) {
  MotionUI::motionUI().initializeTiltToZoom(view, position);
}

void wkext_motion_tilt_stop() {
  MotionUI::motionUI().stopTiltToZoom();
}

int wkext_motion_tilt_is_running() {
  return MotionUI::motionUI().isTiltToZoomStarted();
}

void wkext_motion_tilt_position_update(Evas_Coord_Point* position) {
  MotionUI::motionUI().updateTiltPosition(position);
}

void wkext_motion_tilt_scale_initialize(double scale) {
  MotionUI::motionUI().scaleInitialize(scale);
}

double wkext_motion_tilt_scale_sync(double scale, Wkext_Motion_Type type) {
  int isTiltMode = true;
  if (type == WKEXT_MOTION_TYPE_PINCH)
    isTiltMode = false;

  return MotionUI::motionUI().syncScale(scale, isTiltMode);
}
#else
void wkext_motion_tilt_enable_set(Evas_Object* view, int enable, unsigned int sensitivity) {}
void wkext_motion_tilt_start(Evas_Object* view, const Evas_Coord_Point* position) {}
void wkext_motion_tilt_stop() {}
int wkext_motion_tilt_is_running() { return EINA_FALSE; }
void wkext_motion_tilt_position_update(Evas_Coord_Point* position) {}
void wkext_motion_tilt_scale_initialize(double scale) {}
double wkext_motion_tilt_scale_sync(double scale, Wkext_Motion_Type type) { return 0.; }
#endif
