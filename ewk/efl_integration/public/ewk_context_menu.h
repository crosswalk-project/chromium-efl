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

/**
 * @file    ewk_context_menu.h
 * @brief   Describes the context menu API.
 */

#ifndef ewk_context_menu_h
#define ewk_context_menu_h

#include <Eina.h>
#include "ewk_export.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \enum    Ewk_Context_Menu_Item_Tag
 * @brief   Provides the tags of items for the context menu.
 * @info    Keep this in sync with WKContextMenuItemTypes.h
 */
enum {
    EWK_CONTEXT_MENU_ITEM_TAG_NO_ACTION = 0,
    EWK_CONTEXT_MENU_ITEM_TAG_OPEN_LINK_IN_NEW_WINDOW,
    EWK_CONTEXT_MENU_ITEM_TAG_DOWNLOAD_LINK_TO_DISK,
    EWK_CONTEXT_MENU_ITEM_TAG_COPY_LINK_TO_CLIPBOARD,
    EWK_CONTEXT_MENU_ITEM_TAG_OPEN_IMAGE_IN_NEW_WINDOW,
    EWK_CONTEXT_MENU_ITEM_TAG_DOWNLOAD_IMAGE_TO_DISK,
    EWK_CONTEXT_MENU_ITEM_TAG_COPY_IMAGE_TO_CLIPBOARD,
    EWK_CONTEXT_MENU_ITEM_TAG_OPEN_FRAME_IN_NEW_WINDOW,
    EWK_CONTEXT_MENU_ITEM_TAG_COPY,
    EWK_CONTEXT_MENU_ITEM_TAG_GO_BACK,
    EWK_CONTEXT_MENU_ITEM_TAG_GO_FORWARD,
    EWK_CONTEXT_MENU_ITEM_TAG_STOP,
    EWK_CONTEXT_MENU_ITEM_TAG_RELOAD,
    EWK_CONTEXT_MENU_ITEM_TAG_CUT,
    EWK_CONTEXT_MENU_ITEM_TAG_PASTE,
    EWK_CONTEXT_MENU_ITEM_TAG_SPELLING_GUESS,
    EWK_CONTEXT_MENU_ITEM_TAG_NO_GUESSES_FOUND,
    EWK_CONTEXT_MENU_ITEM_TAG_IGNORE_SPELLING,
    EWK_CONTEXT_MENU_ITEM_TAG_LEARN_SPELLING,
    EWK_CONTEXT_MENU_ITEM_TAG_OTHER,
    EWK_CONTEXT_MENU_ITEM_TAG_SEARCH_IN_SPOTLIGHT,
    EWK_CONTEXT_MENU_ITEM_TAG_SEARCH_WEB,
    EWK_CONTEXT_MENU_ITEM_TAG_LOOK_UP_IN_DICTIONARY,
    EWK_CONTEXT_MENU_ITEM_TAG_OPEN_WITH_DEFAULT_APPLICATION,
    EWK_CONTEXT_MENU_ITEM_TAG_PDF_ACTUAL_SIZE,
    EWK_CONTEXT_MENU_ITEM_TAG_PDF_ZOOM_IN,
    EWK_CONTEXT_MENU_ITEM_TAG_PDF_ZOOM_OUT,
    EWK_CONTEXT_MENU_ITEM_TAG_PDF_AUTO_SIZE,
    EWK_CONTEXT_MENU_ITEM_TAG_PDF_SINGLE_PAGE,
    EWK_CONTEXT_MENU_ITEM_TAG_PDF_FACTING_PAGES,
    EWK_CONTEXT_MENU_ITEM_TAG_PDF_CONTINUOUS,
    EWK_CONTEXT_MENU_ITEM_TAG_PDF_NEXT_PAGE,
    EWK_CONTEXT_MENU_ITEM_TAG_PDF_PREVIOUS_PAGE,
    EWK_CONTEXT_MENU_ITEM_TAG_OPEN_LINK,
    EWK_CONTEXT_MENU_ITEM_TAG_IGNORE_GRAMMAR,
    EWK_CONTEXT_MENU_ITEM_TAG_SPELLING_MENU,
    EWK_CONTEXT_MENU_ITEM_TAG_SHOW_SPELLING_PANEL,
    EWK_CONTEXT_MENU_ITEM_TAG_CHECK_SPELLING,
    EWK_CONTEXT_MENU_ITEM_TAG_CHECK_SPELLING_WHILE_TYPING,
    EWK_CONTEXT_MENU_ITEM_TAG_CHECK_GRAMMAR_WITH_SPELLING,
    EWK_CONTEXT_MENU_ITEM_TAG_FONT_MENU,
    EWK_CONTEXT_MENU_ITEM_TAG_SHOW_FONTS,
    EWK_CONTEXT_MENU_ITEM_TAG_BOLD,
    EWK_CONTEXT_MENU_ITEM_TAG_ITALIC,
    EWK_CONTEXT_MENU_ITEM_TAG_UNDERLINE,
    EWK_CONTEXT_MENU_ITEM_TAG_OUTLINE,
    EWK_CONTEXT_MENU_ITEM_TAG_STYLES,
    EWK_CONTEXT_MENU_ITEM_TAG_SHOW_COLORS,
    EWK_CONTEXT_MENU_ITEM_TAG_SPEECH_MENU,
    EWK_CONTEXT_MENU_ITEM_TAG_START_SPEAKING,
    EWK_CONTEXT_MENU_ITEM_TAG_STOP_SPEAKING,
    EWK_CONTEXT_MENU_ITEM_TAG_WRITING_DIRECTION_MENU,
    EWK_CONTEXT_MENU_ITEM_TAG_DEFAULT_DIRECTION,
    EWK_CONTEXT_MENU_ITEM_TAG_LEFT_TO_RIGHT,
    EWK_CONTEXT_MENU_ITEM_TAG_RIGHT_TO_LEFT,
    EWK_CONTEXT_MENU_ITEM_TAG_PDF_SINGLE_PAGE_SCROLLING,
    EWK_CONTEXT_MENU_ITEM_TAG_PDF_FACING_PAGES_SCROLLING,
    EWK_CONTEXT_MENU_ITEM_TAG_INSPECT_ELEMENT,
    EWK_CONTEXT_MENU_ITEM_TAG_TEXT_DIRECTION_MENU,
    EWK_CONTEXT_MENU_ITEM_TAG_TEXT_DIRECTION_DEFAULT,
    EWK_CONTEXT_MENU_ITEM_TAG_TEXT_DIRECTION_LEFT_TO_RIGHT,
    EWK_CONTEXT_MENU_ITEM_TAG_TEXT_DIRECTION_RIGHT_TO_LEFT,
    EWK_CONTEXT_MENU_ITEM_TAG_CORRECT_SPELLING_AUTOMATICALLY,
    EWK_CONTEXT_MENU_ITEM_TAG_SUBSTITUTIONS_MENU,
    EWK_CONTEXT_MENU_ITEM_TAG_SHOW_SUBSTITUTIONS,
    EWK_CONTEXT_MENU_ITEM_TAG_SMART_COPY_PASTE,
    EWK_CONTEXT_MENU_ITEM_TAG_SMART_QUOTES,
    EWK_CONTEXT_MENU_ITEM_TAG_SMART_DASHES,
    EWK_CONTEXT_MENU_ITEM_TAG_SMART_LINKS,
    EWK_CONTEXT_MENU_ITEM_TAG_TEXT_REPLACEMENT,
    EWK_CONTEXT_MENU_ITEM_TAG_TRANSFORMATIONS_MENU,
    EWK_CONTEXT_MENU_ITEM_TAG_MAKE_UPPER_CASE,
    EWK_CONTEXT_MENU_ITEM_TAG_MAKE_LOWER_CASE,
    EWK_CONTEXT_MENU_ITEM_TAG_CAPITALIZE,
    EWK_CONTEXT_MENU_ITEM_TAG_CHANGE_BACK,
    EWK_CONTEXT_MENU_ITEM_TAG_OPEN_MEDIA_IN_NEW_WINDOW,
    EWK_CONTEXT_MENU_ITEM_TAG_COPY_MEDIA_LINK_TO_CLIPBOARD,
    EWK_CONTEXT_MENU_ITEM_TAG_TOGGLE_MEDIA_CONTROLS,
    EWK_CONTEXT_MENU_ITEM_TAG_TOGGLE_MEDIA_LOOP,
    EWK_CONTEXT_MENU_ITEM_TAG_ENTER_VIDEO_FULLSCREEN,
    EWK_CONTEXT_MENU_ITEM_TAG_MEDIA_PLAY_PAUSE,
    EWK_CONTEXT_MENU_ITEM_TAG_MEDIA_MUTE,
    EWK_CONTEXT_MENU_ITEM_TAG_DICTATION_ALTERNATIVE,
    EWK_CONTEXT_MENU_ITEM_TAG_SELECT_ALL,
    EWK_CONTEXT_MENU_ITEM_TAG_SELECT_WORD,
    EWK_CONTEXT_MENU_ITEM_TAG_TEXT_SELECTION_MODE,
    EWK_CONTEXT_MENU_ITEM_TAG_CLIPBOARD,
    EWK_CONTEXT_MENU_ITEM_TAG_DRAG,
    EWK_CONTEXT_MENU_ITEM_TAG_TRANSLATE,
    EWK_CONTEXT_MENU_ITEM_TAG_COPY_LINK_DATA,
    EWK_CONTEXT_MENU_ITEM_BASE_APPLICATION_TAG = 10000
};
/** Creates a type name for Ewk_Context_Menu_Item_Tag */
typedef uint32_t Ewk_Context_Menu_Item_Tag;

/**
 * \enum    _Ewk_Context_Menu_Item_Type
 * @brief   Defines the types of the items for the context menu.
 * @info    Keep this in sync with WKContextMenuItemTypes.h
 */
enum _Ewk_Context_Menu_Item_Type {
    EWK_CONTEXT_MENU_ITEM_TYPE_ACTION,
    EWK_CONTEXT_MENU_ITEM_TYPE_CHECKABLE_ACTION,
    EWK_CONTEXT_MENU_ITEM_TYPE_SEPARATOR,
    EWK_CONTEXT_MENU_ITEM_TYPE_SUBMENU
};
/** Creates a type name for _Ewk_Context_Menu_Item_Type */
typedef enum _Ewk_Context_Menu_Item_Type Ewk_Context_Menu_Item_Type;

/** Creates a type name for _Ewk_Context_Menu */
typedef struct _Ewk_Context_Menu Ewk_Context_Menu;

/** Creates a type name for _Ewk_Context_Menu_Item */
typedef struct _Ewk_Context_Menu_Item Ewk_Context_Menu_Item;

/**
 * Counts the number of the context menu item.
 *
 * @param menu the context menu object
 *
 * @return number of current context menu item
 */
EAPI unsigned ewk_context_menu_item_count(Ewk_Context_Menu* menu);

/**
 * Returns the nth item in a context menu.
 *
 * @param menu The context menu object
 * @param n The number of the item
 *
 * @return The nth item of context menu
 */
EAPI Ewk_Context_Menu_Item* ewk_context_menu_nth_item_get(Ewk_Context_Menu* menu, unsigned int n);

/**
 * Removes the context menu item from the context menu object.
 *
 * @param menu the context menu object
 * @param item the context menu item to remove
 *
 * @return @c EINA_TRUE on successful request or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_context_menu_item_remove(Ewk_Context_Menu* menu, Ewk_Context_Menu_Item* item);

/**
 * Adds the context menu item to the context menu object.
 *
 * @param menu the context menu object
 * @param tag the tag of context menu item
 * @param title the title of context menu item
 * @param enabled the enabled of context menu item
 *
 * @return @c EINA_TRUE on successful request or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_context_menu_item_append_as_action(Ewk_Context_Menu* menu, Ewk_Context_Menu_Item_Tag tag, const char* title, Eina_Bool enabled);

/**
 * Adds the context menu item to the context menu object.
 *
 * @param menu the context menu object
 * @param tag the tag of context menu item
 * @param title the title of context menu item
 * @param icon_file the path of icon to be set on context menu item
 * @param enabled the enabled of context menu item
 *
 * @return @c EINA_TRUE on successful request or @c EINA_FALSE on failure
 */
EAPI Eina_Bool ewk_context_menu_item_append(Ewk_Context_Menu* menu, Ewk_Context_Menu_Item_Tag tag, const char* title, const char* icon_file, Eina_Bool enabled);

/**
 * Returns the tag of context menu item.
 *
 * @param item The context menu item object
 *
 * @return The tag of context menu item
 */
EAPI Ewk_Context_Menu_Item_Tag ewk_context_menu_item_tag_get(Ewk_Context_Menu_Item* item);

/**
 * Returns the type of context menu item.
 *
 * @param item The context menu item object
 *
 * @return The type of context menu item
 */
EAPI Ewk_Context_Menu_Item_Type ewk_context_menu_item_type_get(Ewk_Context_Menu_Item* item);

/**
 * Returns the item is enabled.
 *
 * @param item The context menu item object to get enabled state
 *
 * @return EINA_TRUE if it is enabled, @c EINA_FALSE if not or on failure
 */
EAPI Eina_Bool ewk_context_menu_item_enabled_get(const Ewk_Context_Menu_Item* item);

/**
 * Returns the link url string of context menu item.
 *
 * @param menu the context menu item object
 *
 * @return current link url string on success or @c 0 on failure
 */
EAPI const char* ewk_context_menu_item_link_url_get(Ewk_Context_Menu_Item* item);

/**
 * Returns the image url string of context menu item.
 *
 * @param menu the context menu item object
 *
 * @return current image url string on success or @c 0 on failure
 */
EAPI const char* ewk_context_menu_item_image_url_get(Ewk_Context_Menu_Item* item);

#ifdef __cplusplus
}
#endif

#endif // ewk_context_menu_h
