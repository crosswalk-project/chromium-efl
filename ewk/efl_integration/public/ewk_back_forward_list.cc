/*
 * Copyright (C) 2012 Intel Corporation. All rights reserved.
 * Copyright (C) 2014 Samsung Electronics. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ewk_back_forward_list.h"
#include "private/ewk_back_forward_list_private.h"

Ewk_Back_Forward_List_Item* ewk_back_forward_list_current_item_get(const Ewk_Back_Forward_List* list)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(list, NULL);
  return chromium_glue::from(static_cast<tizen_webview::BackForwardListItem *>(list->GetCurrentItem()));
}

Ewk_Back_Forward_List_Item *ewk_back_forward_list_previous_item_get(const Ewk_Back_Forward_List *list)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(list, NULL);
  return chromium_glue::from(static_cast<tizen_webview::BackForwardListItem *>(list->GetPrevItem()));
}

Ewk_Back_Forward_List_Item *ewk_back_forward_list_next_item_get(const Ewk_Back_Forward_List *list)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(list, NULL);
  return chromium_glue::from(static_cast<tizen_webview::BackForwardListItem *>(list->GetNextItem()));
}

Ewk_Back_Forward_List_Item *ewk_back_forward_list_item_at_index_get(const Ewk_Back_Forward_List *list, int index)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(list, NULL);
  return chromium_glue::from(static_cast<tizen_webview::BackForwardListItem *>(list->GetItemAtIndex(index)));
}

unsigned ewk_back_forward_list_count(Ewk_Back_Forward_List *list)
{
  // Maybe -1 would be better when list is NULL
  // but 0 is here to be compliant with WebKit-EFL
  EINA_SAFETY_ON_NULL_RETURN_VAL(list, 0);
  return list->GetLength();
}

Eina_List *ewk_back_forward_list_n_back_items_copy(const Ewk_Back_Forward_List *list, int limit)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(list, NULL);
  int count = list->GetBackListLength();
  if (limit > -1 && limit < count) {
    count = limit;
  }
  Eina_List *result = NULL;
  for (int i = 1; i <= count; ++i) {
    result = eina_list_prepend(result, list->GetItemAtIndex(-i));
  }
  return result;
}

Eina_List *ewk_back_forward_list_n_forward_items_copy(const Ewk_Back_Forward_List *list, int limit)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(list, NULL);
  int count = list->GetForwardListLength();
  if (limit > -1 && limit < count) {
    count = limit;
  }
  Eina_List *result = NULL;
  for (int i = 1; i <= count; ++i) {
    result = eina_list_append(result, list->GetItemAtIndex(i));
  }
  return result;
}
