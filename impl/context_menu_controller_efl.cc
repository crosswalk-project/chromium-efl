/*
   Copyright (C) 2013 Samsung Electronics

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

#include "API/ewk_context_menu_private.h"
#include <Elementary.h>
#include "components/clipboard/clipboard_helper_efl.h"
#include "context_menu_controller_efl.h"
#include "eweb_view.h"
#include "ui/base/clipboard/clipboard.h"

namespace content {

ContextMenuControllerEfl::~ContextMenuControllerEfl() {
  if (popup_)
    evas_object_del(popup_);

  Eina_List *ittr;
  Eina_List *next;
  void *data;
  EINA_LIST_FOREACH_SAFE(menu_items_, ittr, next, data) {
    _Ewk_Context_Menu_Item *item = static_cast<_Ewk_Context_Menu_Item*> (data);
    ContextMenuItemEfl *context_item = item->menu_item_;
    menu_items_ =  eina_list_remove(menu_items_, item);
    delete context_item;
    delete item;
  }
}

bool ContextMenuControllerEfl::PopulateAndShowContextMenu(const ContextMenuParams &params) {
  EWebView* view = ToEWebView(evas_object_);
  if (!view)
    return false;

  params_ = params;
  menu_items_ = NULL;
  GetProposedContextMenu();
  _Ewk_Context_Menu *new_menu = new _Ewk_Context_Menu();
  new_menu->menu_list_ = menu_items_;
  view->SmartCallback<EWebViewCallbacks::ContextMenuCustomize>().call(new_menu);
  menu_items_ = new_menu->menu_list_;
  delete new_menu;
  if (!CreateContextMenu())
    return false;

  return ShowContextMenu();
}

void ContextMenuControllerEfl::AddItemToPropsedList(ContextMenuOptionType item,
                                                    ContextMenuOption option,
                                                    std::string title,
                                                    std::string image_url,
                                                    std::string link_url,
                                                    std::string icon_path) {
  ContextMenuItemEfl *new_menu_item = new ContextMenuItemEfl(item, option, title, image_url, link_url, icon_path);
  _Ewk_Context_Menu_Item *new_item = new _Ewk_Context_Menu_Item();
  new_item->menu_item_ = new_menu_item;
  menu_items_ = eina_list_append(menu_items_, new_item);
}

void ContextMenuControllerEfl::GetProposedContextMenu() {
  if (!params_.link_url.is_empty())
    AddItemToPropsedList(MENU_ITEM_TYPE_ACTION,
                         MENU_ITEM_OPEN_LINK_IN_NEW_WINDOW,
                         std::string("Open in new window"),
                         std::string(),
                         params_.link_url.spec(),
                         std::string());
  if (!params_.selection_text.empty())
    AddItemToPropsedList(MENU_ITEM_TYPE_ACTION,
                         MENU_ITEM_CUT,
                         std::string("Cut"),
                         std::string(),
                         std::string(),
                         std::string());
  if (params_.is_editable) {
    AddItemToPropsedList(MENU_ITEM_TYPE_ACTION,
                         MENU_ITEM_SELECT_WORD,
                         std::string("Select"),
                         std::string(),
                         std::string(),
                         std::string());
    AddItemToPropsedList(MENU_ITEM_TYPE_ACTION,
                         MENU_ITEM_SELECT_ALL,
                         std::string("Select All"),
                         std::string(),
                         std::string(),
                         std::string());
    AddItemToPropsedList(MENU_ITEM_TYPE_ACTION,
                         MENU_ITEM_PASTE,
                         std::string("Paste"),
                         std::string(),
                         std::string(),
                         std::string());
  }
  if (!params_.selection_text.empty())
    AddItemToPropsedList(MENU_ITEM_TYPE_ACTION,
                         MENU_ITEM_COPY,
                         std::string("Copy"),
                         std::string(),
                         std::string(),
                         std::string());
  if (params_.has_image_contents && !params_.link_url.is_empty())
    AddItemToPropsedList(MENU_ITEM_TYPE_ACTION,
                         MENU_ITEM_OPEN_IMAGE_IN_NEW_WINDOW,
                         std::string("Open Image"),
                         params_.link_url.spec(),
                         params_.link_url.spec(),
                         std::string());
  if (!params_.has_image_contents && !params_.link_url.is_empty())
    AddItemToPropsedList(MENU_ITEM_TYPE_ACTION,
                         MENU_ITEM_SELECT_LINK,
                         std::string("Select Mode"),
                         params_.link_url.spec(),
                         params_.link_url.spec(),
                         std::string());
  AddItemToPropsedList(MENU_ITEM_TYPE_ACTION,
                       MENU_ITEM_CLIPBOARD,
                       std::string("Clipboard"),
                       std::string(),
                       std::string(),
                       std::string());
}

bool ContextMenuControllerEfl::CreateContextMenu() {
  if (popup_)
    evas_object_del(popup_);

  if (!eina_list_count(menu_items_))
    return false;

  popup_ = elm_ctxpopup_add(evas_object_);
  evas_object_data_set(popup_, "ContextEfl", this);
  elm_object_tree_focus_allow_set(popup_, false);

  Eina_List *ittr;
  void *data;
  EINA_LIST_FOREACH(menu_items_, ittr, data) {
    _Ewk_Context_Menu_Item *item = static_cast<_Ewk_Context_Menu_Item*> (data);
    ContextMenuItemEfl *context_item = item->menu_item_;
    if (!context_item->Title().empty()) {
      if (!context_item->IconPath().empty()) {
        Evas_Object* icon = elm_image_add(popup_);
        elm_image_file_set(icon, context_item->IconPath().c_str(), NULL);
        elm_ctxpopup_item_append(popup_, context_item->Title().c_str(), icon, contextMenuItemSelectedCallback, context_item);
      } else {
        elm_ctxpopup_item_append(popup_, context_item->Title().c_str(), NULL, contextMenuItemSelectedCallback, context_item);
      }
    }
  }
  return true;
}

void ContextMenuControllerEfl::contextMenuCancelCallback(void* data, Evas_Object* obj, void* eventInfo) {
  ContextMenuControllerEfl* menu_controller = static_cast<ContextMenuControllerEfl*>(evas_object_data_get(obj, "ContextEfl"));
  if (menu_controller) {
    evas_object_del(menu_controller->GetPopUp());
    menu_controller->ResetPopUp();
    menu_controller->HideContextMenu();
    evas_object_data_del(obj, "ContextEfl");
  }
}

void ContextMenuControllerEfl::contextMenuItemSelectedCallback(void* data, Evas_Object* obj, void* eventInfo) {
  Evas_Object* pop_up = obj;
#if !defined(OS_TIZEN)
  // In desktop evas_object_data_get(obj) is not working because obj is not the object to which data was attached.
  // So get the parent, obj parent is internal box, box parent is the popup to which object was attached.
  // On target the obj that is obtained is the popup so the data is proper. Need to find why this difference?
  // Temp added this patch so as to work both on target and device.
  Evas_Object* list = elm_object_parent_widget_get(obj);
  pop_up = elm_object_parent_widget_get(list);
#endif
  ContextMenuControllerEfl* menu_controller = static_cast<ContextMenuControllerEfl*>(evas_object_data_get(pop_up, "ContextEfl"));
  if (menu_controller) {
    ContextMenuItemEfl* selected_menu_item = static_cast<ContextMenuItemEfl*>(data);
    if (selected_menu_item)
       menu_controller->MenuItemSelected(selected_menu_item);

    evas_object_del(menu_controller->GetPopUp());
    menu_controller->ResetPopUp();
    menu_controller->HideContextMenu();
    evas_object_data_del(pop_up, "ContextEfl");
  }
}

bool ContextMenuControllerEfl::ShowContextMenu() {
  if (!popup_)
    return false;

  evas_object_move(popup_, params_.x, params_.y);
#if defined(OS_TIZEN)
  // In device context menu is supported as per the platform efl theme.
  // So this supports horizontal popup and on this popup style is applied.
  // But style is not there on desktop version so using default provided by efl
  if (type_ == MENU_TYPE_SELECTION) {
    elm_ctxpopup_horizontal_set(popup_, EINA_TRUE);
    elm_ctxpopup_direction_priority_set(popup_,
                                        ELM_CTXPOPUP_DIRECTION_UP,
                                        ELM_CTXPOPUP_DIRECTION_DOWN,
                                        ELM_CTXPOPUP_DIRECTION_LEFT,
                                        ELM_CTXPOPUP_DIRECTION_RIGHT);
    elm_object_style_set(popup_, "copypaste");
  }
#endif
  evas_object_smart_callback_add(popup_, "dismissed", contextMenuCancelCallback, 0);
  evas_object_show(popup_);
  return true;
}

void ContextMenuControllerEfl::MenuItemSelected(ContextMenuItemEfl *menu_item) {
  EWebView* view = ToEWebView(evas_object_);
  if (!view)
    return;

  // FIXME: Add cases as and when required
  switch(menu_item->GetContextMenuOption())
  {
    case MENU_ITEM_OPEN_LINK_IN_NEW_WINDOW: {
      _Ewk_Context_Menu_Item *item = new _Ewk_Context_Menu_Item();
      item->menu_item_ = menu_item;
      view->SmartCallback<EWebViewCallbacks::ContextMenuItemSelected>().call(item);
      delete item;
      break;
    }
    case MENU_ITEM_GO_BACK: {
      view->GoBack();
      break;
    }
    case MENU_ITEM_GO_FORWARD: {
      view->GoForward();
      break;
    }
    case MENU_ITEM_RELOAD: {
      view->ReloadIgnoringCache();
      break;
    }
    case MENU_ITEM_COPY: {
      view->ExecuteEditCommand("copy", NULL);
      if (params_.is_editable) {
        SelectionControllerEfl* controller = view->GetSelectionController();
        if (controller)
          controller->HideHandle();

        Eina_Rectangle left_rect, right_rect;
        Eina_Bool result = view->GetSelectionRange(&left_rect, &right_rect);
        Evas_Coord x, y;
        evas_object_geometry_get(view->evas_object(), &x, &y, 0, 0);
        right_rect.x += x;
        right_rect.y += y;
        view->MoveCaret(gfx::Point(right_rect.x, right_rect.y));
      } else {
        view->ExecuteEditCommand("Unselect", NULL);
      }
      break;
    }
    case MENU_ITEM_SELECT_LINK: {
      Evas_Coord x, y;
      evas_object_geometry_get(view->evas_object(), &x, &y, 0, 0);
      view->SelectLinkText(gfx::Point(params_.x - x, params_.y - y));
      break;
    }
    case MENU_ITEM_SELECT_WORD: {
      Evas_Coord x, y;
      evas_object_geometry_get(view->evas_object(), &x, &y, 0, 0);
      view->SelectClosestWord(gfx::Point(params_.x - x, params_.y - y));
      SelectionControllerEfl* controller = view->GetSelectionController();
      if (controller)
        controller->HideHandle();
      break;
    }
    case MENU_ITEM_SELECT_ALL: {
      view->ExecuteEditCommand("SelectAll", NULL);
      SelectionControllerEfl* controller = view->GetSelectionController();
      if (controller)
        controller->HideHandle();
      break;
    }
    case MENU_ITEM_PASTE: {
      view->ExecuteEditCommand("paste", NULL);
      break;
    }
    case MENU_ITEM_CUT: {
      view->ExecuteEditCommand("cut", NULL);
      break;
    }
    case MENU_ITEM_CLIPBOARD: {
      //TODO: only text is supported, set false for richly editable.
      // Need to find if view has rich editor to support paste of non-text things.
      ClipboardHelperEfl::GetInstance()->OpenClipboardWindow(view, false);
      break;
    }
    default:
      _Ewk_Context_Menu_Item item;
      item.menu_item_ = menu_item;
      view->SmartCallback<EWebViewCallbacks::ContextMenuItemSelected>().call(&item);
      break;
  }
}

void ContextMenuControllerEfl::HideContextMenu() {
  if (popup_)
    evas_object_del(popup_);

  Eina_List *ittr;
  Eina_List *next;
  void *data;
  EINA_LIST_FOREACH_SAFE(menu_items_, ittr, next, data) {
    _Ewk_Context_Menu_Item *item = static_cast<_Ewk_Context_Menu_Item*> (data);
    ContextMenuItemEfl *context_item = item->menu_item_;
    menu_items_ =  eina_list_remove(menu_items_, item);
    delete context_item;
    delete item;
  }
  menu_items_ = 0;
}

}
