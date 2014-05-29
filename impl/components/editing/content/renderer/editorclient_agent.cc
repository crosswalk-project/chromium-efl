/*
 * Copyright (C) 2013 Samsung Electronics. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 */

#include "components/editing/content/renderer/editorclient_agent.h"
#include "third_party/WebKit/public/web/WebView.h"

#include "content/public/renderer/render_view.h"
#include "components/editing/content/common/editing_messages.h"

using blink::WebEditorClient;
using blink::WebString;

namespace editing {

EditorClientAgent::EditorClientAgent(content::RenderView* render_view)
    : content::RenderViewObserver(render_view) {
  render_view->GetWebView()->setEditorClient(this);
}

bool EditorClientAgent::OnMessageReceived(const IPC::Message& message) {
  return false;
}

void EditorClientAgent::registerUndoStep(size_t undoStackSize) {
  Send(new EditingHostMsg_NotifyRegisterUndo(routing_id(), routing_id(), undoStackSize));
}

void EditorClientAgent::registerRedoStep(size_t redoStackSize) {
  Send(new EditingHostMsg_NotifyRegisterRedo(routing_id(), routing_id(), redoStackSize));
}

void EditorClientAgent::clearUndoRedoOperations(const std::string& command, size_t stackSize) {
  if (command == "Undo")
    Send(new EditingHostMsg_NotifyClearUndo(routing_id(), routing_id(), stackSize));
  else if (command == "Redo")
    Send(new EditingHostMsg_NotifyClearRedo(routing_id(), routing_id(), stackSize));
}

void EditorClientAgent::Undo(size_t undoStackSize) {
  Send(new EditingHostMsg_NotifyUndo(routing_id(), routing_id(), undoStackSize));
}

void EditorClientAgent::Redo(size_t redoStackSize) {
  Send(new EditingHostMsg_NotifyRedo(routing_id(), routing_id(), redoStackSize));
}

}  // namespace editing
