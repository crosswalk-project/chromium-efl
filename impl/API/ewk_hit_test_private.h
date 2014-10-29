// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef EWK_HIT_TEST_PRIVATE_H_
#define EWK_HIT_TEST_PRIVATE_H_

#include <map>
#include <string>

#include <Eina.h>

#include "third_party/skia/include/core/SkBitmap.h"
#include "tizen_webview/public/tw_hit_test.h"

typedef std::map<std::string, std::string> NodeAttributesMap;

// Structure used to report hit test result
struct _Ewk_Hit_Test {
  _Ewk_Hit_Test();
  _Ewk_Hit_Test(const _Ewk_Hit_Test& other);
  // TODO: check whether bitwise assignment is acceptable (e.g, attributeHash)
  // If not, proper implementation for below should be added:
  // _Ewk_Hit_Test& operator=(const _Ewk_Hit_Test&);
  // Note: Currently the operation is used in EwebView::UpdateHitTestData().

  tizen_webview::Hit_Test_Result_Context context;
  std::string linkURI;
  std::string linkTitle; // the title of link
  std::string linkLabel; // the text of the link
  std::string imageURI;
  std::string mediaURI;
  bool isEditable;
  tizen_webview::Hit_Test_Mode mode;

  // store node attributes in a map
  struct Hit_Test_Node_Data {
    Hit_Test_Node_Data();
    Hit_Test_Node_Data(const Hit_Test_Node_Data& other);
    ~Hit_Test_Node_Data();
    void PopulateNodeAtributes(const NodeAttributesMap& nodeAttributes);

    std::string tagName;      // tag name for hit element
    std::string nodeValue;    // node value for hit element
    Eina_Hash* attributeHash; // attribute data for hit element
  } nodeData;

  // when hit node is image we need to store image buffer and filename extension
  struct Hit_Test_Image_Buffer {
    Hit_Test_Image_Buffer();
    Hit_Test_Image_Buffer(const Hit_Test_Image_Buffer& other);

    std::string fileNameExtension; // image filename extension for hit element
    SkBitmap imageBitmap;          // image pixels data
  } imageData;
};

#endif // EWK_HIT_TEST_PRIVATE_H_
