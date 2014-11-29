/*
   Copyright (C) 2012 Samsung Electronics

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#if !defined(EWK_BRINGUP)

#include "ewk_history.h"
#include "private/ewk_private.h"
#include "private/ewk_history_private.h"
#include "Eina.h"

int ewk_history_back_list_length_get(Ewk_History* history)
{
  // returning -1 here would be better to indicate the error
  // but we need to stick to the WebKit-EFL implementation
  EINA_SAFETY_ON_NULL_RETURN_VAL(history, 0);
  history->GetBackListLength();
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
  return chromium_glue::from(history->GetItemAtIndex(index));
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

#endif
