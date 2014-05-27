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

/**
 * @file    ewk_custom_handlers.h
 * @brief   Custom scheme and content handlers. (http://www.w3.org/TR/html5/webappapis.html#custom-handlers)
 */

#ifndef ewk_custom_handlers_h
#define ewk_custom_handlers_h

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _Ewk_Custom_Handlers_Data Ewk_Custom_Handlers_Data;

/// Defines the handler states.
enum _Ewk_Custom_Handlers_State {
    /// Indicates that no attempt has been made to register the given handler.
    EWK_CUSTOM_HANDLERS_NEW,
    /// Indicates that the given handler has been registered or that the site is blocked from registering the handler.
    EWK_CUSTOM_HANDLERS_REGISTERED,
    /// Indicates that the given handler has been offered but was rejected.
    EWK_CUSTOM_HANDLERS_DECLINED
};

/// Creates a type name for @a _Ewk_Custom_Handlers_State.
typedef enum _Ewk_Custom_Handlers_State Ewk_Custom_Handlers_State;

#ifdef __cplusplus
}
#endif
#endif // ewk_custom_handlers_h
