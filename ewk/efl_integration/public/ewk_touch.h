// Copyright 2012 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ewk_touch_h
#define ewk_touch_h

#include <Evas.h>

#ifdef __cplusplus
extern "C" {
#endif

/// Represents types of touch event.
typedef enum {
    EWK_TOUCH_START,
    EWK_TOUCH_MOVE,
    EWK_TOUCH_END,
    EWK_TOUCH_CANCEL
} Ewk_Touch_Event_Type;

/// Creates a type name for Ewk_Touch_Point.
typedef struct _Ewk_Touch_Point Ewk_Touch_Point;

/// Represents a touch point.
struct _Ewk_Touch_Point {
    int id; /**< identifier of the touch event */
    int x; /**< the horizontal position of the touch event */
    int y; /**< the vertical position of the touch event */
    Evas_Touch_Point_State state; /**< state of the touch event */
};

#ifdef __cplusplus
}
#endif

#endif // ewk_touch_h
