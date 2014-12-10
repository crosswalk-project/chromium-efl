// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef RENDER_FRAME_OBSERVER_EFL_H_
#define RENDER_FRAME_OBSERVER_EFL_H_

#include <vector>

#include "content/public/renderer/render_frame_observer.h"

namespace blink {
class WebElement;
}

namespace content {
class RenderFrame;
class RenderFrameImpl;

class RenderFrameObserverEfl : public RenderFrameObserver {
 public:
  explicit RenderFrameObserverEfl(RenderFrame* render_frame);
  virtual ~RenderFrameObserverEfl();

  // IPC::Listener implementation.
  bool OnMessageReceived(const IPC::Message& message) override;

 private:
#if defined(OS_TIZEN)
  void OnSelectPopupMenuItems(bool canceled,
                              const std::vector<int>& selected_indices);
  void OnClosePopupMenu();
#endif

  void OnMovePreviousSelectElement();
  void OnMoveNextSelectElement();
};

} // namespace content

#endif  // RENDER_FRAME_OBSERVER_EFL_H_
