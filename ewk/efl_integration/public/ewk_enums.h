// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ewk_enums_h
#define ewk_enums_h

#ifdef __cplusplus
extern "C" {
#endif

// #if OS(TIZEN)
/// Represents types of gesture.
enum _Ewk_Gesture_Type {
    EWK_GESTURE_TAP,
    EWK_GESTURE_LONG_PRESS,
    EWK_GESTURE_PAN,
    EWK_GESTURE_FLICK,
    EWK_GESTURE_PINCH
};
/// Creates a type name for @a _Ewk_Gesture_Type.
typedef enum _Ewk_Gesture_Type Ewk_Gesture_Type;
// #endif // #if OS(TIZEN)

//#if ENABLE(TIZEN_ORIENTATION_EVENTS)
enum _Ewk_Screen_Orientation {
    EWK_SCREEN_ORIENTATION_PORTRAIT_PRIMARY = 1,
    EWK_SCREEN_ORIENTATION_LANDSCAPE_PRIMARY = 1 << 1,
    EWK_SCREEN_ORIENTATION_PORTRAIT_SECONDARY = 1 << 2,
    EWK_SCREEN_ORIENTATION_LANDSCAPE_SECONDARY = 1 << 3
};
typedef enum _Ewk_Screen_Orientation Ewk_Screen_Orientation;
//#endif

#ifdef __cplusplus
}
#endif

#endif // ewk_enums_h
