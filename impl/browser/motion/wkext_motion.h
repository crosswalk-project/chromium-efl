/*
 *  webkit-extension
 *
 * Copyright (c) 2000 - 2013 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Contact: Taeyun An <ty.an@samsung.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <Evas.h>

#ifndef wkext_motion_h
#define wkext_motion_h

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _Wkext_Motion_Event {
  Evas_Coord_Point position;
  double scale;
} Wkext_Motion_Event;

enum _Wkext_Motion_Type {
  WKEXT_MOTION_TYPE_PINCH,
  WKEXT_MOTION_TYPE_TILT
};
typedef enum _Wkext_Motion_Type Wkext_Motion_Type;

EAPI void wkext_motion_tilt_start(Evas_Object* view, const Evas_Coord_Point* position);
EAPI void wkext_motion_tilt_stop();
EAPI int wkext_motion_tilt_is_running();
EAPI void wkext_motion_tilt_position_update(Evas_Coord_Point* position);
EAPI void wkext_motion_tilt_scale_initialize(double scale);
EAPI double wkext_motion_tilt_scale_sync(double scale, Wkext_Motion_Type type);
EAPI void wkext_motion_tilt_enable_set(Evas_Object *view, int enable, unsigned int sensitivity);
#ifdef __cplusplus
}
#endif

#endif

