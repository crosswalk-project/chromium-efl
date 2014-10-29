// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ewk_context_menu_private_h
#define ewk_context_menu_private_h

#include <Eina.h>
#include "context_menu_controller_efl.h"

// Wrapper for context_menu_controller items
struct _Ewk_Context_Menu {
  Eina_List* menu_list_;
  _Ewk_Context_Menu() : menu_list_(NULL) {}
};

struct _Ewk_Context_Menu_Item {
  content::ContextMenuItemEfl* menu_item_;
  _Ewk_Context_Menu_Item() : menu_item_(NULL) {}
};

#endif // ewk_context_menu_private_h
