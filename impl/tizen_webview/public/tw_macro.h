// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TIZEN_WEBVIEW_PUBLIC_TW_MACRO_H_
#define TIZEN_WEBVIEW_PUBLIC_TW_MACRO_H_

// convenient macros for tizen_webview public headers.

// A macro to disallow the copy constructor and operator= functions
// This should be used in the private: declarations for a class
// Note: extracted from chromium's src/base/macros.h
#ifndef DISALLOW_COPY_AND_ASSIGN
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);               \
  void operator=(const TypeName&)
#endif

#endif  // TIZEN_WEBVIEW_PUBLIC_TW_MACRO_H_
