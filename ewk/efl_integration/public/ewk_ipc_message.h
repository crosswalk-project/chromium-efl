// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

/**
 * @file    ewk_ipc_message.h
 * @brief   Custom support for ipc messages
 */

#ifndef EWK_IPC_MESSAGE_H
#define EWK_IPC_MESSAGE_H

#include <Eina.h>
#include "ewk_export.h"
#include "ewk_context.h"

typedef struct Ewk_Wrt_Message_Data Ewk_IPC_Wrt_Message_Data;

/*
 * Create new Ewk_Wrt_Message_Data object. It has to freed by the caller
 * using ewk_ipc_wrt_message_data_del()
 */
EAPI Ewk_IPC_Wrt_Message_Data* ewk_ipc_wrt_message_data_new();

/*
 * Delete the Ewk_Wrt_Message_Object passed by the caller.
 */
EAPI void ewk_ipc_wrt_message_data_del(Ewk_IPC_Wrt_Message_Data* data);

/*
 * Set message type
 *
 * @return false if NULL string was provided, true otherwise
 */
EAPI Eina_Bool ewk_ipc_wrt_message_data_type_set(Ewk_IPC_Wrt_Message_Data *data, const char *type);

/*
 * Get the type of given message. The string should be released with eina_stringshare_del()
 *
 * @return true on success, false in case provided data structure is NULL
 */
EAPI Eina_Stringshare* ewk_ipc_wrt_message_data_type_get(const Ewk_IPC_Wrt_Message_Data *data);

/*
 * Set message value
 *
 * @return false if NULL string was provided, true otherwise
 */
EAPI Eina_Bool ewk_ipc_wrt_message_data_value_set(Ewk_IPC_Wrt_Message_Data *data, const char *value);

/*
 * Get message value. The string should be released with eina_stringshare_del()
 *
 * @return true on success, false in case provided data structure is NULL
 */
EAPI Eina_Stringshare* ewk_ipc_wrt_message_data_value_get(const Ewk_IPC_Wrt_Message_Data *data);

/*
 * Set message ID
 *
 * @return false if NULL string was provided, true otherwise
 */
EAPI Eina_Bool ewk_ipc_wrt_message_data_id_set(Ewk_IPC_Wrt_Message_Data *data, const char *id);

/*
 * Get message ID. The string should be released with eina_stringshare_del()
 *
 * @return true on success, false in case provided data structure is NULL
 */
EAPI Eina_Stringshare* ewk_ipc_wrt_message_data_id_get(const Ewk_IPC_Wrt_Message_Data *data);

/*
 * Get message reference ID
 *
 * @return false if NULL string was provided, true otherwise
 */
EAPI Eina_Bool ewk_ipc_wrt_message_data_reference_id_set(Ewk_IPC_Wrt_Message_Data *data, const char *reference_id);

/*
 * Set message reference ID. The string should be released with eina_stringshare_del()
 *
 * @return true on success, false in case provided data structure is NULL
 */
EAPI Eina_Stringshare* ewk_ipc_wrt_message_data_reference_id_get(const Ewk_IPC_Wrt_Message_Data *data);

/**
 *  Send IPC message to Wrt
 *
 *  Plugins -> Wrt (Renderer->Browser)
 */
EAPI Eina_Bool ewk_ipc_plugins_message_send(int routingId, const Ewk_IPC_Wrt_Message_Data* data);

/**
 *   Send IPC message to Plugins
 *
 *   Wrt -> Plugins (Browser->Renderer)
 *   //TODO - it is always send to all rendereres - it need to be modified
 */
EAPI Eina_Bool ewk_ipc_wrt_message_send(Ewk_Context* context, const Ewk_IPC_Wrt_Message_Data* data);


/**
 *  Send Synchronous IPC message to Wrt
 *
 *  Plugins -> Wrt (answer: -> Plugins) (Renderer->Browser -> Renderer)
 *
 *  If success the value member will be set by the handler
 */
EAPI Eina_Bool ewk_ipc_plugins_sync_message_send(int routingId, Ewk_IPC_Wrt_Message_Data* data);


#endif // EWK_IPC_MESSAGE_H
