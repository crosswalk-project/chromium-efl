// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ewk_context_menu.h"

#include <Eina.h>

// TODO: remove dependency
#include <../impl/API/ewk_context_menu_private.h>
#include <../impl/context_menu_controller_efl.h>

unsigned ewk_context_menu_item_count(Ewk_Context_Menu* menu)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(menu, 0);
  return eina_list_count(menu->GetMenuList());
}

Ewk_Context_Menu_Item* ewk_context_menu_nth_item_get(Ewk_Context_Menu* menu, unsigned int n)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(menu, NULL);
  return static_cast<Ewk_Context_Menu_Item*>(eina_list_nth(menu->GetMenuList(), n));
}

Eina_Bool ewk_context_menu_item_remove(Ewk_Context_Menu* menu, Ewk_Context_Menu_Item* item)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(menu, false);
  EINA_SAFETY_ON_NULL_RETURN_VAL(item, false);

  menu->RemoveItem(item);
  return true;
}

Eina_Bool ewk_context_menu_item_append_as_action(Ewk_Context_Menu* menu,
    Ewk_Context_Menu_Item_Tag tag, const char* title, Eina_Bool enabled)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(menu, false);
  EINA_SAFETY_ON_NULL_RETURN_VAL(title, false);
  content::ContextMenuItemEfl *new_item = new content::ContextMenuItemEfl(
      content::MENU_ITEM_TYPE_ACTION,
      static_cast<content::ContextMenuOption>(tag),
      std::string(title));
  new_item->SetEnabled(enabled);
  _Ewk_Context_Menu_Item *item = new _Ewk_Context_Menu_Item(new_item);
  menu->AppendItem(item);
  return true;
}


Eina_Bool ewk_context_menu_item_append(Ewk_Context_Menu* menu,
    Ewk_Context_Menu_Item_Tag tag, const char* title, const char* icon_file,
    Eina_Bool enabled)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(menu, false);
  EINA_SAFETY_ON_NULL_RETURN_VAL(title, false);
  EINA_SAFETY_ON_NULL_RETURN_VAL(icon_file, false);

  content::ContextMenuItemEfl *new_item = new content::ContextMenuItemEfl(
      content::MENU_ITEM_TYPE_ACTION,
      static_cast<content::ContextMenuOption>(tag),
      std::string(title),
      std::string(),
      std::string(),
      std::string(icon_file));
  new_item->SetEnabled(enabled);
  _Ewk_Context_Menu_Item *item = new _Ewk_Context_Menu_Item(new_item);
  menu->AppendItem(item);
  return true;
}

Ewk_Context_Menu_Item_Tag ewk_context_menu_item_tag_get(Ewk_Context_Menu_Item* item)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(item, 0);
  content::ContextMenuItemEfl *info_item = item->GetMenuItem();
  return static_cast<Ewk_Context_Menu_Item_Tag>(info_item->GetContextMenuOption());
}

Ewk_Context_Menu_Item_Type ewk_context_menu_item_type_get(Ewk_Context_Menu_Item* item)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(item, EWK_CONTEXT_MENU_ITEM_TYPE_ACTION);
  content::ContextMenuItemEfl *info_item = item->GetMenuItem();
  return static_cast<Ewk_Context_Menu_Item_Type>(info_item->GetContextMenuOptionType());
}

Eina_Bool ewk_context_menu_item_enabled_get(const Ewk_Context_Menu_Item* item)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(item, false);
  content::ContextMenuItemEfl *info_item = item->GetMenuItem();
  return info_item->IsEnabled();
}

const char* ewk_context_menu_item_link_url_get(Ewk_Context_Menu_Item* item)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(item, NULL);
  content::ContextMenuItemEfl *info_item = item->GetMenuItem();
  return info_item->LinkURL().c_str();
}

const char* ewk_context_menu_item_image_url_get(Ewk_Context_Menu_Item* item)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(item, NULL);
  content::ContextMenuItemEfl *info_item = item->GetMenuItem();
  return info_item->ImageURL().c_str();
}
