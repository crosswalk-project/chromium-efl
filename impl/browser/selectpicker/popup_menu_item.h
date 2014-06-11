/*
 * Copyright (C) 2014 Samsung Electronics
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef popup_menu_item_h
#define popup_menu_item_h

#include "ewk_view.h"
#include <Eina.h>
#include <string>
#include "third_party/WebKit/Source/platform/text/TextDirection.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Enum values containing type of popup menu item. */
typedef enum {
    POPUP_MENU_OPTION,
    POPUP_MENU_CHECKABLE_OPTION,
    POPUP_MENU_GROUP,
    POPUP_MENU_SEPARATOR,
    POPUP_MENU_SUBMENU,
    POPUP_MENU_UNKNOWN = -1
} Popup_Menu_Item_Type;

/** Creates a type name for Ewk_Popup_Menu_Item */
typedef struct Popup_Menu_Item Popup_Menu_Item;

/**
 * Returns type of the popup menu item.
 *
 * @param item the popup menu item instance
 *
 * @return the type of the @a item or @c EWK_POPUP_MENU_UNKNOWN in case of error.
 */
Popup_Menu_Item_Type popup_menu_item_type_get(const Popup_Menu_Item *item);

/**
 * Returns text of the popup menu item.
 *
 * @param item the popup menu item instance
 *
 * @return the text of the @a item or @c NULL in case of error. This pointer is
 *         guaranteed to be eina_stringshare, so whenever possible
 *         save yourself some cpu cycles and use
 *         eina_stringshare_ref() instead of eina_stringshare_add() or
 *         strdup()
 */
const char *popup_menu_item_text_get(const Popup_Menu_Item *item);

/**
 * Returns text direction of the popup menu item.
 *
 * @param item the popup menu item instance
 *
 * @return the text direction of the @a item.
 */
WebCore::TextDirection popup_menu_item_text_direction_get(const Popup_Menu_Item *item);

/**
 * Returns whether the popup menu item has text direction override.
 *
 * @param item the popup menu item instance
 *
 * @return @c EINA_TRUE if the popup menu item has text direction override,
 *         @c EINA_FALSE otherwise.
 */
Eina_Bool popup_menu_item_text_direction_override_get(const Popup_Menu_Item *item);

/**
 * Returns tooltip of the popup menu item.
 *
 * @param item the popup menu item instance
 *
 * @return the tooltip of the @a item or @c NULL in case of error. This pointer is
 *         guaranteed to be eina_stringshare, so whenever possible
 *         save yourself some cpu cycles and use
 *         eina_stringshare_ref() instead of eina_stringshare_add() or
 *         strdup()
 */
const char *popup_menu_item_tooltip_get(const Popup_Menu_Item *item);

/**
 * Returns accessibility text of the popup menu item.
 *
 * @param item the popup menu item instance
 *
 * @return the accessibility text of the @a item or @c NULL in case of error.
 *         This pointer is guaranteed to be eina_stringshare, so whenever
 *         possible save yourself some cpu cycles and use
 *         eina_stringshare_ref() instead of eina_stringshare_add() or
 *         strdup()
 */
const char *popup_menu_item_accessibility_text_get(const Popup_Menu_Item *item);

/**
 * Returns whether the popup menu item is enabled or not.
 *
 * @param item the popup menu item instance
 *
 * @return @c EINA_TRUE if the popup menu item is enabled, @c EINA_FALSE otherwise.
 */
Eina_Bool popup_menu_item_enabled_get(const Popup_Menu_Item *item);

/**
 * Returns whether the popup menu item is label or not.
 *
 * @param item the popup menu item instance
 *
 * @return @c EINA_TRUE if the popup menu item is label, @c EINA_FALSE otherwise.
 */
Eina_Bool popup_menu_item_is_label_get(const Popup_Menu_Item *item);

/**
 * Returns whether the popup menu item is selected or not.
 *
 * @param item the popup menu item instance
 *
 * @return @c EINA_TRUE if the popup menu item is selected, @c EINA_FALSE otherwise.
 */
Eina_Bool popup_menu_item_selected_get(const Popup_Menu_Item *item);

#ifdef __cplusplus
}
#endif
#endif // popup_menu_item_h
