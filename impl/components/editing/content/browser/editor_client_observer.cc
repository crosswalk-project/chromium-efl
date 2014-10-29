// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "editor_client_observer.h"

#include <stddef.h>
#include <limits>
#include <map>
#include <set>
#include <utility>

#include "base/bind.h"
#include "base/strings/string16.h"
#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/render_view_host.h"

#include "eweb_view.h"
#include "web_contents_delegate_efl.h"
#include "common/web_contents_utils.h"
#include "components/editing/content/common/editing_messages.h"

namespace editing {

EWebView* GetWebViewFromId(int render_process_id, int render_view_id) {
  content::WebContents* web_contents = web_contents_utils::WebContentsFromViewID(render_process_id,
                                                                                        render_view_id);
  if (!web_contents)
    return NULL;

  content::WebContentsDelegateEfl* delegate = static_cast<content::WebContentsDelegateEfl*>(web_contents->
                                                                                            GetDelegate());
  if (!delegate)
    return NULL;
  return (delegate->web_view());
}

EditorClientObserver::EditorClientObserver(int render_process_id)
    : BrowserMessageFilter(ViewMsgStart)
    , render_process_id_(render_process_id) {
}

void EditorClientObserver::OverrideThreadForMessage(const IPC::Message& message,
                                                    content::BrowserThread::ID* thread) {
  switch (message.type()) {
    case EditingHostMsg_NotifyUndo::ID:
    case EditingHostMsg_NotifyRedo::ID:
    case EditingHostMsg_NotifyRegisterUndo::ID:
    case EditingHostMsg_NotifyRegisterRedo::ID:
    case EditingHostMsg_NotifyClearUndo::ID:
    case EditingHostMsg_NotifyClearRedo::ID:
      *thread = content::BrowserThread::UI;
      break;
    default:
      break;
  }
}

bool EditorClientObserver::OnMessageReceived(const IPC::Message& message) {
  bool handled = true;
  IPC_BEGIN_MESSAGE_MAP(EditorClientObserver, message)
    IPC_MESSAGE_HANDLER(EditingHostMsg_NotifyUndo,
                        OnUndoNotify)
    IPC_MESSAGE_HANDLER(EditingHostMsg_NotifyRedo,
                        OnRedoNotify)
    IPC_MESSAGE_HANDLER(EditingHostMsg_NotifyRegisterUndo,
                        OnRegisterUndoNotify)
    IPC_MESSAGE_HANDLER(EditingHostMsg_NotifyRegisterRedo,
                        OnRegisterRedoNotify)
    IPC_MESSAGE_HANDLER(EditingHostMsg_NotifyClearUndo,
                        OnClearUndoStackNotify)
    IPC_MESSAGE_HANDLER(EditingHostMsg_NotifyClearRedo,
                        OnClearRedoStackNotify)
    IPC_MESSAGE_UNHANDLED(handled = false)
  IPC_END_MESSAGE_MAP()
  return handled;
}

void EditorClientObserver::OnUndoNotify(int render_view_id, size_t undoStackSize) {
  EWebView* web_view = GetWebViewFromId(render_process_id_, render_view_id);
  if (web_view)
    web_view->SmartCallback<EWebViewCallbacks::UndoSize>().call(&undoStackSize);
}

void EditorClientObserver::OnRedoNotify(int render_view_id, size_t redoStackSize) {
  EWebView* web_view = GetWebViewFromId(render_process_id_, render_view_id);
  if (web_view)
    web_view->SmartCallback<EWebViewCallbacks::RedoSize>().call(&redoStackSize);
}

void EditorClientObserver::OnRegisterUndoNotify(int render_view_id, size_t undoStackSize) {
  EWebView* web_view = GetWebViewFromId(render_process_id_, render_view_id);
  if (web_view)
    web_view->SmartCallback<EWebViewCallbacks::UndoSize>().call(&undoStackSize);
}

void EditorClientObserver::OnRegisterRedoNotify(int render_view_id, size_t redoStackSize) {
  EWebView* web_view = GetWebViewFromId(render_process_id_, render_view_id);
  if (web_view)
    web_view->SmartCallback<EWebViewCallbacks::RedoSize>().call(&redoStackSize);
}

void EditorClientObserver::OnClearUndoStackNotify(int render_view_id, size_t undoStackSize) {
  EWebView* web_view = GetWebViewFromId(render_process_id_, render_view_id);
  if (web_view)
    web_view->SmartCallback<EWebViewCallbacks::UndoSize>().call(&undoStackSize);
}

void EditorClientObserver::OnClearRedoStackNotify(int render_view_id, size_t redoStackSize) {
  EWebView* web_view = GetWebViewFromId(render_process_id_, render_view_id);
  if (web_view)
    web_view->SmartCallback<EWebViewCallbacks::RedoSize>().call(&redoStackSize);
}

}  // namespace editing
