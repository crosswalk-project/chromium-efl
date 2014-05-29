/*
 * Copyright (C) 2014 Samsung Electronics
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef EWK_HIT_TEST_PRIVATE_H_
#define EWK_HIT_TEST_PRIVATE_H_

#include <map>
#include <string>

#include <Eina.h>

#include "third_party/skia/include/core/SkBitmap.h"

#include "public/ewk_hit_test.h"

typedef std::map<std::string, std::string> NodeAttributesMap;

// Structure used to report hit test result
struct _Ewk_Hit_Test {
  _Ewk_Hit_Test();
  _Ewk_Hit_Test(const _Ewk_Hit_Test& other);

  Ewk_Hit_Test_Result_Context context;
  std::string linkURI;
  std::string linkTitle; // the title of link
  std::string linkLabel; // the text of the link
  std::string imageURI;
  std::string mediaURI;
  bool isEditable;
  Ewk_Hit_Test_Mode mode;

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
