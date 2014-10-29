// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ewk_hit_test_private.h"

using namespace tizen_webview;

static void FreeHitTestAttributeHashData(void* data) {
   eina_stringshare_del(static_cast<Eina_Stringshare*>(data));
 }

_Ewk_Hit_Test::_Ewk_Hit_Test()
  : context(TW_HIT_TEST_RESULT_CONTEXT_DOCUMENT),
    isEditable(false),
    mode(TW_HIT_TEST_MODE_DEFAULT) {
}

_Ewk_Hit_Test::_Ewk_Hit_Test(const _Ewk_Hit_Test& other)
    : context(other.context),
      linkURI(other.linkURI),
      linkTitle(other.linkTitle),
      linkLabel(other.linkLabel),
      imageURI(other.imageURI),
      mediaURI(other.mediaURI),
      isEditable(other.isEditable),
      mode(other.mode),
      nodeData(other.nodeData),
      imageData(other.imageData) {
}

_Ewk_Hit_Test::Hit_Test_Node_Data::Hit_Test_Node_Data()
    : attributeHash(NULL) {
}

// deep copy of node map
_Ewk_Hit_Test::Hit_Test_Node_Data::Hit_Test_Node_Data(const Hit_Test_Node_Data& other)
    : tagName(other.tagName),
      nodeValue(other.nodeValue),
      attributeHash(NULL) {
  if (!other.attributeHash)
    return;

  Eina_Iterator* it = eina_hash_iterator_tuple_new(other.attributeHash);
  void* data;
  attributeHash = eina_hash_string_superfast_new(FreeHitTestAttributeHashData);
  while (eina_iterator_next(it, &data)) {
    Eina_Hash_Tuple* t = static_cast<Eina_Hash_Tuple*>(data);
    eina_hash_add(attributeHash,
      static_cast<const char*>(t->key), eina_stringshare_add(static_cast<const char*>(t->data)));
  }
  eina_iterator_free(it);
}

_Ewk_Hit_Test::Hit_Test_Node_Data::~Hit_Test_Node_Data() {
  eina_hash_free(attributeHash);
}

void _Ewk_Hit_Test::Hit_Test_Node_Data::PopulateNodeAtributes(const NodeAttributesMap& nodeAttributes) {
  if (nodeAttributes.empty())
    return;

  attributeHash = eina_hash_string_superfast_new(FreeHitTestAttributeHashData);
  NodeAttributesMap::const_iterator attributeMapEnd = nodeAttributes.end();
  for (NodeAttributesMap::const_iterator it = nodeAttributes.begin(); it != attributeMapEnd; ++it) {
    eina_hash_add(attributeHash, it->first.c_str(), eina_stringshare_add(it->second.c_str()));
  }
}

_Ewk_Hit_Test::Hit_Test_Image_Buffer::Hit_Test_Image_Buffer() {
}

//(warning) deep copy of skia buffer.
_Ewk_Hit_Test::Hit_Test_Image_Buffer::Hit_Test_Image_Buffer(const Hit_Test_Image_Buffer& other)
    : fileNameExtension(other.fileNameExtension) {
  other.imageBitmap.deepCopyTo(&imageBitmap);
}
