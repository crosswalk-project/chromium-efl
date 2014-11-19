// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ewk_context_menu_private_h
#define ewk_context_menu_private_h

#include <Eina.h>
#include <stdio.h>

#include "context_menu_controller_efl.h"

class _Ewk_Context_Menu_Item {
 public:
  _Ewk_Context_Menu_Item(content::ContextMenuItemEfl* menu_item)
      : menu_item_(menu_item) {
  }

  ~_Ewk_Context_Menu_Item() {
    delete menu_item_;
  }

  content::ContextMenuItemEfl* GetMenuItem() const {
    return menu_item_;
  }
 private:
  content::ContextMenuItemEfl* menu_item_;
};


// Wrapper for context_menu_controller items
class _Ewk_Context_Menu {
 public:
  _Ewk_Context_Menu(Eina_List* menu_list)
      : menu_list_(menu_list) {
  }

  ~_Ewk_Context_Menu() {
    if (menu_list_) {
      void* data;
      EINA_LIST_FREE(menu_list_, data) {
        _Ewk_Context_Menu_Item *item = static_cast<_Ewk_Context_Menu_Item*> (data);
        delete item;
      }
    }
  }

  const Eina_List* GetMenuList() const {
    return menu_list_;
  }

  Eina_List* TakeMenuList() {
    Eina_List* ptr = menu_list_;
    menu_list_ = NULL;
    return ptr;
  }

  void AppendItem(const void* item) {
    menu_list_ = eina_list_append(menu_list_, item);
  }

  void RemoveItem(_Ewk_Context_Menu_Item* item) {
    menu_list_ = eina_list_remove(menu_list_, item);
    delete item;
  }

 private:
  Eina_List* menu_list_;
};

#endif // ewk_context_menu_private_h
