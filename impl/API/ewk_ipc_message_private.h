/*
 * Copyright (C) 2014 Samsung Electronics
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
 *
 */

#ifndef EWK_IPC_MESSAGE_PRIVATE_H
#define EWK_IPC_MESSAGE_PRIVATE_H

#include <Eina.h>
#include <string>

#include "tizen_webview/public/tw_web_context.h"

struct Ewk_IPC_Wrt_Message_Data {
  std::string type;
  std::string value;

  std::string id;
  std::string reference_id;
};

Eina_Bool ewk_ipc_plugins_message_send_private(int routingId, const Ewk_IPC_Wrt_Message_Data& data);
Eina_Bool ewk_ipc_wrt_message_send_private(tizen_webview::WebContext* context, const Ewk_IPC_Wrt_Message_Data& data);
Eina_Bool ewk_ipc_plugins_sync_message_send_private(int routingId, Ewk_IPC_Wrt_Message_Data* data);

#endif // EWK_IPC_MESSAGE_PRIVATE_H
