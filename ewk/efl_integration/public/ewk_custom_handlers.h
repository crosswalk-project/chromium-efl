// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/**
 * @file    ewk_custom_handlers.h
 * @brief   Custom scheme and content handlers. (http://www.w3.org/TR/html5/webappapis.html#custom-handlers)
 */

#ifndef ewk_custom_handlers_h
#define ewk_custom_handlers_h

#include <Eina.h>
#include <Evas.h>
#include "ewk_export.h"

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

/**
 * Get target(scheme or mime type) of custom handlers.
 *
 * @param data custom handlers's structure.
 *
 * @return @c target (scheme or mime type).
 */
EAPI Eina_Stringshare* ewk_custom_handlers_data_target_get(const Ewk_Custom_Handlers_Data* data);

/**
 * Get base url of custom handlers.
 *
 * @param data custom handlers's structure.
 *
 * @return @c base url.
 */
EAPI Eina_Stringshare* ewk_custom_handlers_data_base_url_get(const Ewk_Custom_Handlers_Data* data);

/**
 * Get url of custom handlers.
 *
 * @param data custom handlers's structure.
 *
 * @return @c url.
 */
EAPI Eina_Stringshare* ewk_custom_handlers_data_url_get(const Ewk_Custom_Handlers_Data* data);

/**
 * Get title of custom handlers.
 *
 * @param data custom handlers's structure.
 *
 * @return @c title.
 */
EAPI Eina_Stringshare* ewk_custom_handlers_data_title_get(const Ewk_Custom_Handlers_Data* data);

/**
 * Set result of isProtocolRegistered API.
 *
 * @param data custom handlers's structure
 * @param result(Ewk_Custom_Handlers_State) of isProtocolRegistered and isContentRegistered API
 */
EAPI void ewk_custom_handlers_data_result_set(Ewk_Custom_Handlers_Data* data, Ewk_Custom_Handlers_State result);
#ifdef __cplusplus
}
#endif
#endif // ewk_custom_handlers_h
