// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TIZEN_WEBVIEW_PUBLIC_TW_HIT_TEST_MODE_H_
#define TIZEN_WEBVIEW_PUBLIC_TW_HIT_TEST_MODE_H_

namespace tizen_webview {

/**
 * Hit_Test_Mode:
 * @TW_HIT_TEST_MODE_DEFAULT: link data.
 * @TW_HIT_TEST_MODE_NODE_DATA: extra node data(tag name, node value, attribute information, etc).
 * @TW_HIT_TEST_MODE_IMAGE_DATA: extra image data(image data, image data length, image file name extension, etc).
 * @TW_HIT_TEST_MODE_ALL: all data.
 *
 * Enum values with flags representing the hit test mode.
 */
enum Hit_Test_Mode {
  TW_HIT_TEST_MODE_DEFAULT = 1 << 1,
  TW_HIT_TEST_MODE_NODE_DATA = 1 << 2,
  TW_HIT_TEST_MODE_IMAGE_DATA = 1 << 3,
  TW_HIT_TEST_MODE_ALL = TW_HIT_TEST_MODE_DEFAULT | TW_HIT_TEST_MODE_NODE_DATA | TW_HIT_TEST_MODE_IMAGE_DATA
};

/**
 * Hit_Test_Result_Context:
 * @TW_HIT_TEST_RESULT_CONTEXT_DOCUMENT: anywhere in the document.
 * @TW_HIT_TEST_RESULT_CONTEXT_LINK: a hyperlink element.
 * @TW_HIT_TEST_RESULT_CONTEXT_IMAGE: an image element.
 * @TW_HIT_TEST_RESULT_CONTEXT_MEDIA: a video or audio element.
 * @TW_HIT_TEST_RESULT_CONTEXT_SELECTION: the area is selected by the user.
 * @TW_HIT_TEST_RESULT_CONTEXT_EDITABLE: the area is editable by the user.
 * @TW_HIT_TEST_RESULT_CONTEXT_TEXT: the area is text.
 *
 * Enum values with flags representing the context of a #Ewk_Hit_Test.
 */
enum Hit_Test_Result_Context {
  TW_HIT_TEST_RESULT_CONTEXT_DOCUMENT = 1 << 1,
  TW_HIT_TEST_RESULT_CONTEXT_LINK = 1 << 2,
  TW_HIT_TEST_RESULT_CONTEXT_IMAGE = 1 << 3,
  TW_HIT_TEST_RESULT_CONTEXT_MEDIA = 1 << 4,
  TW_HIT_TEST_RESULT_CONTEXT_SELECTION = 1 << 5,
  TW_HIT_TEST_RESULT_CONTEXT_EDITABLE = 1 << 6,
  TW_HIT_TEST_RESULT_CONTEXT_TEXT = 1 << 7
};

} // namespace tizen_webview

#endif  // TIZEN_WEBVIEW_PUBLIC_TW_HIT_TEST_MODE_H_
