// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef popup_picker_h
#define popup_picker_h

#include <Evas.h>

typedef struct _Elm_Object_Item Elm_Object_Item;
typedef struct Popup_Menu_Item Popup_Menu_Item;

namespace WebKit {
class WebPopupMenuProxyEfl;
}

class EWebView;

struct _Popup_Picker {
    _Popup_Picker()
        : container(NULL)
        , parent(NULL)
        , popupList(NULL)
        , win(NULL)
        , web_view_(NULL)
        , radioMain(NULL)
        , m_genlist_callback_data_list(NULL)
        , firstItem(NULL)
        , changedList(NULL) {}
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
    _genlist_callback_data() : user_data(NULL), it(NULL), menuItem(NULL) {}
    int index;
    void* user_data;
    Elm_Object_Item* it;
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
