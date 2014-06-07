// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TIZEN_WEBVIEW_PUBLIC_TW_INPUT_TYPE_H_
#define TIZEN_WEBVIEW_PUBLIC_TW_INPUT_TYPE_H_

namespace tizen_webview {

// Provides type of focused input element
enum Input_Type {
  TW_INPUT_TYPE_TEXT,
  TW_INPUT_TYPE_TELEPHONE,
  TW_INPUT_TYPE_NUMBER,
  TW_INPUT_TYPE_EMAIL,
  TW_INPUT_TYPE_URL,
  TW_INPUT_TYPE_PASSWORD,
  TW_INPUT_TYPE_COLOR,
  TW_INPUT_TYPE_DATE,
  TW_INPUT_TYPE_DATETIME,
  TW_INPUT_TYPE_DATETIMELOCAL,
  TW_INPUT_TYPE_MONTH,
  TW_INPUT_TYPE_TIME,
  TW_INPUT_TYPE_WEEK
};

} // namespace tizen_webview

#endif  // TIZEN_WEBVIEW_PUBLIC_TW_INPUT_TYPE_H_
