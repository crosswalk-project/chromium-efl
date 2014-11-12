// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TW_BACK_FORWARD_HISTORY_H
#define TW_BACK_FORWARD_HISTORY_H

#include "tw_back_forward_list.h"
#include "base/strings/utf_string_conversions.h"
#include "content/public/browser/navigation_controller.h"
#include "content/public/browser/navigation_entry.h"

namespace tizen_webview {


class BackForwardHistoryItem :
    public back_forward_list::Item {
 public:
  BackForwardHistoryItem(content::NavigationEntry* entry) {
    if (entry) {
      url_ = entry->GetURL();
      original_url_ = entry->GetUserTypedURL();
      title_ = base::UTF16ToUTF8(entry->GetTitle());
    }
  }

  const GURL& GetURL() const {
    return url_;
  }

  const GURL& GetOriginalURL() const {
    return original_url_;
  }

  const std::string& GetTitle() const {
    return title_;
  }

 private:
  GURL url_;
  GURL original_url_;
  std::string title_;
};

class BackForwardHistory :
    public back_forward_list::List {
 public:
  BackForwardHistory(content::NavigationController &controller);
  ~BackForwardHistory();

  int GetCurrentIndex() const;
  int GetLength() const;
  int GetBackListLength() const;
  int GetForwardListLength() const;
  back_forward_list::Item* GetItemAtIndex(int index) const;

 private:
  const int current_index_;
  std::vector<BackForwardHistoryItem*> items_;
};

}

#endif // TW_BACK_FORWARD_HISTORY_H
