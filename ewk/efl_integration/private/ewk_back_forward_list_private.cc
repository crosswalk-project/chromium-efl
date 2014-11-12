// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ewk_back_forward_list_private.h"

_Ewk_Back_Forward_List* chromium_glue::from(
    tizen_webview::BackForwardList* list) {
  return static_cast<_Ewk_Back_Forward_List*>(list);
}

_Ewk_Back_Forward_List_Item* chromium_glue::from(
    tizen_webview::BackForwardListItem* item) {
  return static_cast<_Ewk_Back_Forward_List_Item*>(item);
}
