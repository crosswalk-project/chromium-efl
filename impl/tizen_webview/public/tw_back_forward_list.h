// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TW_BACK_FORWARD_LIST_H
#define TW_BACK_FORWARD_LIST_H

#include "url/gurl.h"

namespace tizen_webview {

namespace back_forward_list {

class Item {
 public:
  virtual ~Item() {}
  // Returns actually loaded URL.
  virtual const GURL& GetURL() const = 0;
  // Returns user typed URL.
  virtual const GURL& GetOriginalURL() const = 0;
  // Returns page title.
  virtual const std::string& GetTitle() const = 0;
};

class List {
 public:
  virtual ~List() {}
  // Returns current position in back-forward list
  // which is -1 when list is empty.
  virtual int GetCurrentIndex() const = 0;

  // Returns the length of the list.
  virtual int GetLength() const = 0;

  // Returns number of back items available from
  // current position.
  // Returns 0 when the list is empty - to be compatible
  // with WebKit-EFL implementation.
  virtual int GetBackListLength() const = 0;

  // Returns number od forward items available from
  // current position.
  // Returns 0 when the list is empty - to be compatible
  // with WebKit-EFL implementation.
  virtual int GetForwardListLength() const = 0;

  // Returns item at given index, while index 0 points to
  // the current item, back items are returned for negative indices
  // and forward items are returned for positive indices.
  virtual Item* GetItemAtIndex(int index) const = 0;

  // Returns item preceding current item.
  virtual Item* GetPrevItem() const {
    return GetItemAtIndex(-1);
  }

  // Returns item following current item.
  virtual Item* GetNextItem() const {
    return GetItemAtIndex(1);
  }

  // Returns current item.
  virtual Item* GetCurrentItem() const {
    return GetItemAtIndex(0);
  }
};

} // namespace back_forward_list

} // namespace tizen_webview

#endif // TW_BACK_FORWARD_LIST_H
