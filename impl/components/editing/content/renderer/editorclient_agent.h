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

#ifndef COMPONENTS_EDITING_CONTENT_RENDERER_EDITORCLIENT_AGENT_H_
#define COMPONENTS_EDITING_CONTENT_RENDERER_EDITORCLIENT_AGENT_H_

#include <string>

#include "base/basictypes.h"
#include "content/public/renderer/render_view_observer.h"
#include "third_party/WebKit/public/web/WebEditorClient.h"

namespace editing {

class EditorClientAgent : public content::RenderViewObserver,
                          public blink::WebEditorClient {
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
