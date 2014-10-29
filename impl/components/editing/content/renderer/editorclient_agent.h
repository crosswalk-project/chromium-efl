// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_EDITING_CONTENT_RENDERER_EDITORCLIENT_AGENT_H_
#define COMPONENTS_EDITING_CONTENT_RENDERER_EDITORCLIENT_AGENT_H_

#include <string>

#include "base/basictypes.h"
#include "content/public/renderer/render_view_observer.h"
#if !defined(EWK_BRINGUP)
#include "third_party/WebKit/public/web/WebEditorClient.h"
#endif

namespace editing {

#if !defined(EWK_BRINGUP)
class EditorClientAgent : public content::RenderViewObserver,
                          public blink::WebEditorClient {
#else
class EditorClientAgent : public content::RenderViewObserver {
#endif
 public:
  EditorClientAgent(content::RenderView* render_view);

  // RenderView::Observer:
  virtual bool OnMessageReceived(const IPC::Message& message) OVERRIDE;

 private:
  // blink::WebEditorClient:
  virtual void registerUndoStep(size_t undoStackSize);
  virtual void registerRedoStep(size_t redoStackSize);
  virtual void clearUndoRedoOperations(const std::string& command, size_t stackSize);
  virtual void Undo(size_t undoStackSize);
  virtual void Redo(size_t redoStackSize);
};

} // namespace editing

#endif // COMPONENTS_EDITING_CONTENT_RENDERER_EDITORCLIENT_AGENT_H_
