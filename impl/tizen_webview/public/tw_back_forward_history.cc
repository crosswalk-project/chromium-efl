// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "tw_back_forward_history.h"

namespace tizen_webview {

BackForwardHistory::BackForwardHistory(
    content::NavigationController &controller)
    : current_index_(controller.GetCurrentEntryIndex()) {
  int cnt = controller.GetEntryCount();
  if (cnt) {
    items_.resize(cnt, 0);
    for (int i = 0; i < cnt; ++i) {
      BackForwardHistoryItem* item = new BackForwardHistoryItem(
          controller.GetEntryAtIndex(i));
      items_[i] = item;
    }
  }
}

BackForwardHistory::~BackForwardHistory() {
  for (unsigned i = 0; i < items_.size(); ++i) {
    delete items_[i];
  }
}

int BackForwardHistory::GetCurrentIndex() const {
  return current_index_;
}

int BackForwardHistory::GetLength() const {
  return items_.size();
}

int BackForwardHistory::GetBackListLength() const {
  return current_index_ < 0 ? 0 : current_index_;
}

int BackForwardHistory::GetForwardListLength() const {
  return items_.size() - current_index_ - 1;
}

back_forward_list::Item* BackForwardHistory::GetItemAtIndex(int index) const {
  int actualIndex = index + current_index_;
  if (actualIndex < 0 || actualIndex >= static_cast<int>(items_.size())) {
    return 0;
  }
  return items_[actualIndex];
}

}
