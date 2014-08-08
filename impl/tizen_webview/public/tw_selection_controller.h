// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TIZEN_WEBVIEW_PUBLIC_TW_SELECTION_CONTROLLER_H_
#define TIZEN_WEBVIEW_PUBLIC_TW_SELECTION_CONTROLLER_H_

namespace tizen_webview {

class SelectionController {
 public:
  virtual bool TextSelectionDown(int x, int y) = 0;
  virtual bool TextSelectionUp(int x, int y) = 0;
  virtual bool IsAnyHandleVisible() const =0;
  virtual void HideHandle() = 0;
  virtual ~SelectionController() = 0;
};

} // namespace tizen_webview

#endif  // TIZEN_WEBVIEW_PUBLIC_TW_SELECTION_CONTROLLER_H_
