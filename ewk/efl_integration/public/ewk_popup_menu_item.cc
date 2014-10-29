// Copyright 2012 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ewk_popup_menu_item.h"
#include "private/ewk_private.h"

Ewk_Popup_Menu_Item_Type ewk_popup_menu_item_type_get(const Ewk_Popup_Menu_Item* item)
{
  LOG_EWK_API_MOCKUP();
  return EWK_POPUP_MENU_UNKNOWN;
}

const char* ewk_popup_menu_item_text_get(const Ewk_Popup_Menu_Item* item)
{
  LOG_EWK_API_MOCKUP();
  return NULL;
}

Eina_Bool ewk_popup_menu_item_selected_get(const Ewk_Popup_Menu_Item* item)
{
  LOG_EWK_API_MOCKUP();
  return false;
}
