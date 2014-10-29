// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_EDITING_CONTENT_BROWSER_EDITOR_CLIENT_OBSERVER_H_
#define COMPONENTS_EDITING_CONTENT_BROWSER_EDITOR_CLIENT_OBSERVER_H_

#include "content/public/browser/browser_message_filter.h"

namespace content {
class RenderViewHost;
class WebContents;
}

namespace editing {

class EditorClientObserver : public content::BrowserMessageFilter {
 public:
  EditorClientObserver(int render_process_id);

  virtual bool OnMessageReceived(const IPC::Message& message) OVERRIDE;
  virtual void OverrideThreadForMessage(const IPC::Message& message,
                                        content::BrowserThread::ID* thread) OVERRIDE;
 private:
  void OnUndoNotify(int render_view_id, size_t undoStackSize);
  void OnRedoNotify(int render_view_id, size_t redoStackSize);
  void OnRegisterUndoNotify(int render_view_id, size_t undoStackSize);
  void OnRegisterRedoNotify(int render_view_id, size_t redoStackSize);
  void OnClearUndoStackNotify(int render_view_id, size_t undoStackSize);
  void OnClearRedoStackNotify(int render_view_id, size_t redoStackSize);
  int render_process_id_;
};

} // namespace editing

#endif  // COMPONENTS_EDITING_CONTENT_BROWSER_EDITOR_CLIENT_OBSERVER_H_
