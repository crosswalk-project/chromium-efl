// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
