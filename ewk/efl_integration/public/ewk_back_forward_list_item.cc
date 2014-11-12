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

#include "ewk_back_forward_list_item.h"
#include "private/ewk_back_forward_list_private.h"

Ewk_Back_Forward_List_Item *ewk_back_forward_list_item_ref(Ewk_Back_Forward_List_Item *item)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(item, NULL);
  item->AddRef();
  return item;
}

void ewk_back_forward_list_item_unref(Ewk_Back_Forward_List_Item *item)
{
  EINA_SAFETY_ON_NULL_RETURN(item);
  item->Release();
}

const char *ewk_back_forward_list_item_url_get(const Ewk_Back_Forward_List_Item *item)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(item, NULL);
  return eina_stringshare_add(item->GetURL().spec().c_str());
}

const char *ewk_back_forward_list_item_title_get(const Ewk_Back_Forward_List_Item *item)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(item, NULL);
  return eina_stringshare_add(item->GetTitle().c_str());
}

const char *ewk_back_forward_list_item_original_url_get(const Ewk_Back_Forward_List_Item *item)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(item, NULL);
  return eina_stringshare_add(item->GetOriginalURL().spec().c_str());
}
