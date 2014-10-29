// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "public/ewk_hit_test.h"

#include "tizen_webview/public/tw_hit_test.h"

// TODO: remove dependency
#include <../impl/eweb_view.h>
#include <../impl/web_contents_delegate_efl.h>

#include "public/ewk_view.h"
#include "private/chromium_glue.h"
#include "private/ewk_hit_test_private.h"

using namespace tizen_webview;

void ewk_hit_test_free(Ewk_Hit_Test* hitTest)
{
  delete hitTest;
}

Ewk_Hit_Test_Result_Context ewk_hit_test_result_context_get(Ewk_Hit_Test* hitTest)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(hitTest, EWK_HIT_TEST_RESULT_CONTEXT_DOCUMENT);
  return chromium_glue::from(hitTest->GetResultContext());
}

const char* ewk_hit_test_link_uri_get(Ewk_Hit_Test* hitTest)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(hitTest, 0);
  return hitTest->GetLinkUri();
}

const char* ewk_hit_test_link_title_get(Ewk_Hit_Test* hitTest)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(hitTest, 0);
  return hitTest->GetLinkTitle();
}

const char* ewk_hit_test_link_label_get(Ewk_Hit_Test* hitTest)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(hitTest, 0);
  return hitTest->GetLinkLabel();
}

const char* ewk_hit_test_image_uri_get(Ewk_Hit_Test* hitTest)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(hitTest, 0);
  return hitTest->GetImageUri();
}

const char* ewk_hit_test_media_uri_get(Ewk_Hit_Test* hitTest)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(hitTest, 0);
  return hitTest->GetMediaUri();
}

const char* ewk_hit_test_tag_name_get(Ewk_Hit_Test* hitTest)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(hitTest, 0);
  return hitTest->GetNodeTagName();
}

const char* ewk_hit_test_node_value_get(Ewk_Hit_Test* hitTest)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(hitTest, 0);
  return hitTest->GetNodeValue();
}

Eina_Hash* ewk_hit_test_attribute_hash_get(Ewk_Hit_Test* hitTest)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(hitTest, 0);
  return hitTest->GetNodeAttributeHash();
}

void* ewk_hit_test_image_buffer_get(Ewk_Hit_Test* hitTest)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(hitTest, 0);
  return hitTest->GetImageBuffer();
}

unsigned int ewk_hit_test_image_buffer_length_get(Ewk_Hit_Test* hitTest)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(hitTest, 0);
  return hitTest->GetImageBufferLength();
}

const char* ewk_hit_test_image_file_name_extension_get(Ewk_Hit_Test* hitTest)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(hitTest, 0);
  return hitTest->GetImageFilenameExtension();
}

int FOR_TESTING_ewk_hit_test_image_width_get(Ewk_Hit_Test* hit_test)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(hit_test, 0);
  return hit_test->GetImageWidth();
}

int FOR_TESTING_ewk_hit_test_image_height_get(Ewk_Hit_Test* hit_test)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(hit_test, 0);
  return hit_test->GetImageHeight();
}
