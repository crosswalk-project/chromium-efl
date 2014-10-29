// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TIZEN_WEBVIEW_PUBLIC_TW_HIT_TEST_MODE_H_
#define TIZEN_WEBVIEW_PUBLIC_TW_HIT_TEST_MODE_H_

#include <eina_hash.h>
#include <cstddef>   // Provides size_t

struct _Ewk_Hit_Test;
class EWebView;

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


class Hit_Test {
 public:
  Hit_Test();
  Hit_Test(const Hit_Test& other);
  ~Hit_Test();
  Hit_Test& operator=(const Hit_Test& other);

  Hit_Test_Result_Context GetResultContext() const;

  const char* GetLinkUri() const;
  const char* GetLinkTitle() const;
  const char* GetLinkLabel() const;

  const char* GetImageUri() const;
  const char* GetImageFilenameExtension() const;
  int         GetImageWidth() const;
  int         GetImageHeight() const;
  void*       GetImageBuffer() const;
  size_t      GetImageBufferLength() const;

  const char* GetMediaUri() const;

  const char* GetNodeTagName() const;
  const char* GetNodeValue() const;
  Eina_Hash*  GetNodeAttributeHash() const;

 private:
  typedef _Ewk_Hit_Test Hit_Test_Impl;
  Hit_Test_Impl* impl;

  // construct directly from the impl class;
  explicit Hit_Test(const Hit_Test_Impl& impl_);

  // utility function
  void Swap(Hit_Test& other);

  friend class ::EWebView;
};

} // namespace tizen_webview

#endif  // TIZEN_WEBVIEW_PUBLIC_TW_HIT_TEST_MODE_H_
