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

#ifndef popup_picker_h
#define popup_picker_h

#include <Evas.h>

typedef struct _Elm_Object_Item Elm_Object_Item;
typedef struct _Eina_List Eina_List;
typedef struct Popup_Menu_Item Popup_Menu_Item;
typedef struct _Eina_Inarray Eina_Inarray;

namespace WebKit {
class WebPopupMenuProxyEfl;
}

class EWebView;

struct _Popup_Picker {
    Evas_Object* container;
    Evas_Object* parent;
    Evas_Object* popupList;
    Evas_Object* win;
    EWebView* web_view_;
    Evas_Object* radioMain;
    Eina_List* m_genlist_callback_data_list;
    Elm_Object_Item* firstItem;
    int selectedIndex;
    Eina_Bool multiSelect;
    Eina_Inarray* changedList;
};
typedef struct _Popup_Picker Popup_Picker;

typedef struct _genlist_callback_data {
    int index;
    void *user_data;
    Elm_Object_Item *it;
    Popup_Menu_Item* menuItem;
} genlist_callback_data;

Popup_Picker* popup_picker_new(EWebView* web_view, Evas_Object* parent, Eina_List* items, int selectedIndex, Eina_Bool multiple);
void popup_picker_resize(Popup_Picker* picker);
void popup_picker_del(Popup_Picker* picker);
void popup_picker_update(Evas_Object* parent, Popup_Picker* picker, Eina_List* items, int selectedIndex);
void listClosed(void *data, Evas_Object *obj, const char *emission, const char *source);
void popup_picker_buttons_update(Popup_Picker* picker, int position, int count, bool enable);
void view_popup_menu_select(EWebView* web_view, int selectedIndex);
void view_popup_menu_multiple_select(EWebView* web_view, Eina_Inarray* changeList);
void view_popup_menu_close(EWebView* web_view);

#endif // popup_picker_h
