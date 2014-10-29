// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ewk_ipc_message_private.h"

#include "content/public/renderer/render_view.h"
#include "common/render_messages_efl.h"
#include "eweb_context.h"

Eina_Bool ewk_ipc_plugins_message_send_private(int routingId, const Ewk_IPC_Wrt_Message_Data &data) {
  content::RenderView* render_view = content::RenderView::FromRoutingID(routingId);
  if (!render_view) {
    return false;
  }

  render_view->Send(new EwkHostMsg_WrtMessage(render_view->GetRoutingID(), data));

  return true;
}

Eina_Bool ewk_ipc_wrt_message_send_private(tizen_webview::WebContext* context, const Ewk_IPC_Wrt_Message_Data &data) {
  EWebContext* web_context = context->GetImpl();
  if (!web_context) {
    return false;
  }
  web_context->SendWrtMessage(data);
  return true;
}

Eina_Bool ewk_ipc_plugins_sync_message_send_private(int routingId, Ewk_IPC_Wrt_Message_Data* data) {
  content::RenderView* render_view = content::RenderView::FromRoutingID(routingId);
  if (!render_view) {
    return false;
  }

  std::string result;
  render_view->Send(new EwkHostMsg_WrtSyncMessage(routingId, *data, &result));
  data->value.swap(result);

  return true;
}
