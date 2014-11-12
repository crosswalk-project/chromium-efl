// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/public/browser/notification_types.h"
#include "content/public/browser/notification_service.h"
#include "content/public/browser/navigation_details.h"
#include "tizen_webview/public/tw_back_forward_list_impl.h"

namespace tizen_webview {

BackForwardList::BackForwardList(content::NavigationController &controller)
    : navigation_controller_(controller) {
  notification_registrar_.Add(this, content::NOTIFICATION_NAV_ENTRY_CHANGED,
      content::NotificationService::AllBrowserContextsAndSources());
  notification_registrar_.Add(this,
      content::NOTIFICATION_WEB_CONTENTS_TITLE_UPDATED,
      content::NotificationService::AllBrowserContextsAndSources());
  notification_registrar_.Add(this, content::NOTIFICATION_NAV_ENTRY_COMMITTED,
      content::NotificationService::AllBrowserContextsAndSources());
}

int BackForwardList::GetCurrentIndex() const {
  return navigation_controller_.GetCurrentEntryIndex();
}

int BackForwardList::GetLength() const {
  return navigation_controller_.GetEntryCount();
}

int BackForwardList::GetBackListLength() const {
  int current = navigation_controller_.GetCurrentEntryIndex();
  return current < 0 ? 0 : current;
}

int BackForwardList::GetForwardListLength() const {
  int current = navigation_controller_.GetCurrentEntryIndex();
  return navigation_controller_.GetEntryCount() - current - 1;
}

back_forward_list::Item* BackForwardList::GetItemAtIndex(int index) const {
  index += navigation_controller_.GetCurrentEntryIndex();
  int count = navigation_controller_.GetEntryCount();
  if (index < 0 || index >= count) {
    return NULL;
  }
  return FindOrCreateItem(index);
}

void BackForwardList::NewPageCommited(int prev_entry_index,
                                      content::NavigationEntry* new_entry) {
  int current = prev_entry_index + 1;
  if (current != navigation_controller_.GetCurrentEntryIndex()) {
    return;
  }

  // When user went back several pages and now loaded
  // some new page (so new entry is commited) then all forward items
  // were deleted, so we have to do the same in our cache.
  for (unsigned i = current; i < indexes_.size(); ++i) {
    content::NavigationEntry* entry = indexes_[i];
    indexes_[i] = NULL;
    if (entry) {
      cache_.erase(entry);
    }
  }

  InsertEntryToIndexes(current, new_entry);
  cache_[new_entry] = scoped_refptr<BackForwardListItem>(new BackForwardListItem(new_entry));
}

void BackForwardList::UpdateItemWithEntry(
    const content::NavigationEntry* entry) {
  if (entry) {
    CacheMap::iterator it = cache_.find(entry);
    if (it != cache_.end()) {
      it->second->Update(entry);
    }
  }
}

void BackForwardList::ClearCache() {
  indexes_.clear();
  cache_.clear();
}

void BackForwardList::Observe(int type,
                              const content::NotificationSource &source,
                              const content::NotificationDetails &details) {
  switch (static_cast<content::NotificationType>(type)) {
    case content::NOTIFICATION_NAV_ENTRY_COMMITTED: {
      content::Details<content::LoadCommittedDetails> d = details;
      NewPageCommited(d->previous_entry_index, d->entry);
      break;
    }
    case content::NOTIFICATION_NAV_ENTRY_CHANGED: {
      content::Details<content::EntryChangedDetails> d = details;
      const content::NavigationEntry* entry = d->changed_entry;
      UpdateItemWithEntry(entry);
      break;
    }
    case content::NOTIFICATION_WEB_CONTENTS_TITLE_UPDATED: {
      content::Details<std::pair<content::NavigationEntry*, bool> > d = details;
      const content::NavigationEntry* entry = d->first;
      UpdateItemWithEntry(entry);
      break;
    }
    default: {
      return;
    }
  }
}

BackForwardListItem* BackForwardList::FindOrCreateItem(int index) const {
  content::NavigationEntry* entry =
      navigation_controller_.GetEntryAtIndex(index);

  if (!entry) {
    return NULL;
  }

  BackForwardListItem* item = NULL;
  CacheMap::iterator it = cache_.find(entry);
  if (it != cache_.end()) {
    // item already in cache
    item = it->second.get();
  } else {
    // need to create new item
    item = new BackForwardListItem(entry);
    cache_[entry] = scoped_refptr<BackForwardListItem>(item);
  }

  InsertEntryToIndexes(index, entry);

  return item;
}

void BackForwardList::InsertEntryToIndexes(
    unsigned index, content::NavigationEntry* entry) const {
  if (index == indexes_.size()) {
    indexes_.push_back(entry);
    return;
  }

  if (index > indexes_.size()) {
    indexes_.resize(index + 1, NULL);
  }
  indexes_[index] = entry;
}

} // namespace tizen_webview
