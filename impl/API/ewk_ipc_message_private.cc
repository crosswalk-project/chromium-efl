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
