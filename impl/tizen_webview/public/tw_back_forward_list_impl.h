// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TW_BACK_FORWARD_LIST_IMPL_H
#define TW_BACK_FORWARD_LIST_IMPL_H

#include "tw_back_forward_list.h"
#include "base/memory/ref_counted.h"
#include "base/strings/string16.h"
#include "base/strings/utf_string_conversions.h"
#include "content/public/browser/navigation_controller.h"
#include "content/public/browser/navigation_entry.h"
#include "content/public/browser/notification_observer.h"
#include "content/public/browser/notification_registrar.h"

#include <map>

namespace tizen_webview {

class BackForwardListItem :
    public back_forward_list::Item,
    public base::RefCounted<BackForwardListItem> {
 public:
  BackForwardListItem(content::NavigationEntry* entry) {
    Update(entry);
  }

  void Update(const content::NavigationEntry* entry) {
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

class BackForwardList :
    public back_forward_list::List,
    public content::NotificationObserver {
 public:
  typedef std::map<const content::NavigationEntry*,
      scoped_refptr<BackForwardListItem> > CacheMap;

  BackForwardList(content::NavigationController &controller);
  ~BackForwardList() {}

  int GetCurrentIndex() const;
  int GetLength() const;
  int GetBackListLength() const;
  int GetForwardListLength() const;
  back_forward_list::Item* GetItemAtIndex(int index) const;

  void NewPageCommited(int prev_entry_index,
                       content::NavigationEntry* new_entry);
  void UpdateItemWithEntry(const content::NavigationEntry* entry);
  void ClearCache();

  void Observe(int type, const content::NotificationSource &source,
               const content::NotificationDetails &details);

 private:
  BackForwardListItem* FindOrCreateItem(int index) const;
  void InsertEntryToIndexes(unsigned index,
                            content::NavigationEntry* entry) const;

 private:
  content::NavigationController &navigation_controller_;
  content::NotificationRegistrar notification_registrar_;
  mutable CacheMap cache_;
  mutable std::vector<content::NavigationEntry*> indexes_;
};

}

#endif // TW_BACK_FORWARD_LIST_IMPL_H
