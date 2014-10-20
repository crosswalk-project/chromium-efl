/*
 * Copyright (C) 2013 Samsung Electronics
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

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
