// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "tw_hit_test.h"
#include <algorithm>
#include "API/ewk_hit_test_private.h"

namespace tizen_webview {

Hit_Test::Hit_Test()
    : impl (new _Ewk_Hit_Test) {
}

Hit_Test::Hit_Test(const Hit_Test& other)
    : impl (new _Ewk_Hit_Test(*other.impl)) {
}

Hit_Test::Hit_Test(const Hit_Test_Impl& impl_)
    : impl (new _Ewk_Hit_Test(impl_)) {
}

Hit_Test::~Hit_Test() {
  delete impl;
}

Hit_Test& Hit_Test::operator=(const Hit_Test& other) {
  if (this != &other) {
    Hit_Test temp (other);
    Swap(temp);
  }
  return *this;
}

void Hit_Test::Swap(Hit_Test& other) {
  std::swap(impl, other.impl);
}

tizen_webview::Hit_Test_Result_Context Hit_Test::GetResultContext() const {
  return impl->context;
}

const char* Hit_Test::GetLinkUri() const {
  return impl->linkURI.c_str();
}

const char* Hit_Test::GetLinkTitle() const {
  return impl->linkTitle.c_str();
}

const char* Hit_Test::GetLinkLabel() const {
  return impl->linkLabel.c_str();
}

const char* Hit_Test::GetImageUri() const {
  return impl->imageURI.c_str();
}

const char* Hit_Test::GetImageFilenameExtension() const {
  return impl->imageData.fileNameExtension.c_str();
}

int Hit_Test::GetImageWidth() const {
  return impl->imageData.imageBitmap.width();
}

int Hit_Test::GetImageHeight() const {
 return  impl->imageData.imageBitmap.height();
}

void* Hit_Test::GetImageBuffer() const {
  return impl->imageData.imageBitmap.getPixels();
}

size_t Hit_Test::GetImageBufferLength() const {
  return impl->imageData.imageBitmap.getSize();
}

const char* Hit_Test::GetMediaUri() const {
  return impl->mediaURI.c_str();
}

const char* Hit_Test::GetNodeTagName() const {
  return impl->nodeData.tagName.c_str();
}

const char* Hit_Test::GetNodeValue() const {
  return impl->nodeData.nodeValue.c_str();
}

Eina_Hash* Hit_Test::GetNodeAttributeHash() const {
  return impl->nodeData.attributeHash;
}

} // namespace tizen_webview
