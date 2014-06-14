// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "tw_wrt.h"

#include "content/public/renderer/render_view.h"
#include "common/render_messages_efl.h"
#include "eweb_context.h"
#include "tizen_webview/public/tw_web_context.h"

namespace tizen_webview {

const char* WrtIpcMessageData::GetType() const {
  return type.c_str();
}

void WrtIpcMessageData::SetType(const char* val) {
  type = val;
}

const char* WrtIpcMessageData::GetValue() const {
  return value.c_str();
}

void WrtIpcMessageData::SetValue(const char* val) {
  value = val;
}

const char* WrtIpcMessageData::GetId() const {
  return id.c_str();
}

void WrtIpcMessageData::SetId(const char* val) {
  id = val;
}

const char* WrtIpcMessageData::GetReferenceId() const {
  return reference_id.c_str();
}

void WrtIpcMessageData::SetReferenceId(const char* val) {
  reference_id = val;
}


bool WrtIpc::SendMessage(WebContext* ctx, const WrtIpcMessageData& data) {
  EWebContext* web_context = ctx->GetImpl();
  if (!web_context) {
    return false;
  }
  web_context->SendWrtMessage(data);
  return true;
}

bool WrtPluginsIpc::SendMessage(int routing_id, const WrtIpcMessageData& data) {
  content::RenderView* rv = content::RenderView::FromRoutingID(routing_id);
  if (!rv) {
     return false;
  }
  rv->Send(new EwkHostMsg_WrtMessage(rv->GetRoutingID(), data));
  return true;
}

bool WrtPluginsIpc::SendSyncMessage(int routing_id, WrtIpcMessageData* data) {
  content::RenderView* rv = content::RenderView::FromRoutingID(routing_id);
  if (!rv) {
    return false;
  }

  rv->Send(new EwkHostMsg_WrtSyncMessage(routing_id, *data, &data->value));
  return true;
}

} // namespace tizen_webview
