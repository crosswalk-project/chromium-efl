// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TIZEN_WEBVIEW_PUBLIC_TW_REF_COUNTED_H_
#define TIZEN_WEBVIEW_PUBLIC_TW_REF_COUNTED_H_

// This is a simplified version of chromium's base::RefCounted class
// (reference : src/base/memory/ref_counted.h).
// The reason why not just use chromium's class is to make tizen_webview public
// headers independent to chromium's internal headers and, as a result,
// tizen chromium-efl package could be kept self-contained.

#include "tizen_webview/public/tw_macro.h"

namespace tizen_webview {
//
// A base class for reference counted classes.  Otherwise, known as a cheap
// knock-off of WebKit's RefCounted<T> class.  To use this guy just extend your
// class from it like so:
//
//   class MyFoo : public base::RefCounted<MyFoo> {
//    ...
//    private:
//     friend class base::RefCounted<MyFoo>;
//     ~MyFoo();
//   };
//
// You should always make your destructor private, to avoid any code deleting
// the object accidently while there are references to it.
template <class T>
class RefCounted {
 public:
  bool HasOneRef() const { return ref_count_ == 1; }

  void AddRef() const { ++ref_count_; }

  void Release() const {
    if (--ref_count_ == 0)
      delete static_cast<const T*>(this);
  }

 protected:
  RefCounted() : ref_count_(0) { }

  ~RefCounted() { }

 private:
  mutable int ref_count_;
  DISALLOW_COPY_AND_ASSIGN(RefCounted<T>);
};

} // namespace tizen_webview

#endif  // TIZEN_WEBVIEW_PUBLIC_TW_REF_COUNTED_H_
