// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ewk_history.h"
#include "private/ewk_private.h"
#include "private/ewk_history_private.h"
#include "Eina.h"

int ewk_history_back_list_length_get(Ewk_History* history)
{
  // returning -1 here would be better to indicate the error
  // but we need to stick to the WebKit-EFL implementation
  EINA_SAFETY_ON_NULL_RETURN_VAL(history, 0);
  return history->GetBackListLength();
}

int ewk_history_forward_list_length_get(Ewk_History* history)
{
  // returning -1 here would be better to indicate the error
  // but we need to stick to the WebKit-EFL implementation
  EINA_SAFETY_ON_NULL_RETURN_VAL(history, 0);
  return history->GetForwardListLength();
}

Ewk_History_Item* ewk_history_nth_item_get(Ewk_History* history, int index)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(history, NULL);
  return chromium_glue::from(static_cast<tizen_webview::BackForwardHistoryItem *>(history->GetItemAtIndex(index)));
}

const char* ewk_history_item_uri_get(Ewk_History_Item* item)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(item, NULL);
  // GetURL().spec() returns const reference
  return item->GetURL().spec().c_str();
}

const char* ewk_history_item_title_get(Ewk_History_Item* item)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(item, NULL);
  // GetTitle() return const reference
  return item->GetTitle().c_str();
}

void ewk_history_free(Ewk_History* history)
{
  EINA_SAFETY_ON_NULL_RETURN(history);
  delete history;
}
