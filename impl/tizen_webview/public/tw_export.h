// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TIZEN_WEBVIEW_PUBLIC_TW_EXPORT_H_
#define TIZEN_WEBVIEW_PUBLIC_TW_EXPORT_H_

#if defined(WIN32)

#if defined(TIZEN_WEBVIEW_IMPLEMENTATION)
#define TW_EXPORT __declspec(dllexport)
#else
#define TW_EXPORT __declspec(dllimport)
#endif  // defined(TIZEN_WEBVIEW_IMPLEMENTATION)

#else // defined(WIN32)
#if defined(TIZEN_WEBVIEW_IMPLEMENTATION)
#define TW_EXPORT __attribute__((visibility("default")))
#else
#define TW_EXPORT
#endif
#endif

#endif  // TIZEN_WEBVIEW_PUBLIC_TW_EXPORT_H_
