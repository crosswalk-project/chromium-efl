// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TIZEN_WEBVIEW_PUBLIC_TW_FIND_OPTIONS_H_
#define TIZEN_WEBVIEW_PUBLIC_TW_FIND_OPTIONS_H_

namespace tizen_webview {

// Enum values used to specify search options.
// @brief   Provides option to find text
// @info    Keep this in sync with ewk_view.h
enum Find_Options {
  TW_FIND_OPTIONS_NONE,                         // no search flags, this means a case sensitive, no wrap, forward only search
  TW_FIND_OPTIONS_CASE_INSENSITIVE = 1 << 0,    // case insensitive search
  TW_FIND_OPTIONS_BACKWARDS = 1 << 3,           // search backwards
  TW_FIND_OPTIONS_AT_WORD_STARTS = 1 << 1,      // search text only at the beginning of the words
  TW_FIND_OPTIONS_TREAT_MEDIAL_CAPITAL_AS_WORD_START = 1 << 2,
                                                // treat capital letters in the middle of words as word start
  TW_FIND_OPTIONS_WRAP_AROUND = 1 << 4,         // if not present search will stop at the end of the document
  TW_FIND_OPTIONS_SHOW_OVERLAY = 1 << 5,        // show overlay
  TW_FIND_OPTIONS_SHOW_FIND_INDICATOR = 1 << 6, // show indicator
  TW_FIND_OPTIONS_SHOW_HIGHLIGHT = 1 << 7       // show highlight
};

} // namespace tizen_webview

#endif  // TIZEN_WEBVIEW_PUBLIC_TW_FIND_OPTIONS_H_
