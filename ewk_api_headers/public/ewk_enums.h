/*
 * Copyright (C) 2013 Samsung Electronics
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef ewk_enums_h
#define ewk_enums_h

#ifdef __cplusplus
extern "C" {
#endif

enum _Ewk_Gesture_Type {
    EWK_GESTURE_TAP,
    EWK_GESTURE_LONG_PRESS,
    EWK_GESTURE_PAN,
    EWK_GESTURE_FLICK,
    EWK_GESTURE_PINCH
};
typedef enum _Ewk_Gesture_Type Ewk_Gesture_Type;

#ifdef __cplusplus
}
#endif

#endif // ewk_enums_h
