// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "popup_picker.h"
#include "popup_menu_item.h"

// DJKim : FIXME
//#include "WebPopupMenuProxyEfl.h"
#include "eweb_view.h"
#include "base/path_service.h"
#include "base/files/file_path.h"
#include "paths_efl.h"
#include "base/logging.h"

#include <Elementary.h>
#include <libintl.h>
#include <Ecore_X.h>
#ifdef OS_TIZEN_MOBILE
#include <dlfcn.h>
#include <efl_assist.h>
extern void* EflAssistHandle;
#endif

int compareChangedItems(const void* a, const void* b) {
  const int* left = static_cast<const int*>(a);
  const int* right = static_cast<const int*>(b);
  return (*left - *right);
}

static void __picker_radio_icon_changed_cb(void* data, Evas_Object* obj, void* event_info) {
  genlist_callback_data *callback_data = static_cast<genlist_callback_data*>(data);
  Popup_Picker* picker = static_cast<Popup_Picker*>(callback_data->user_data);

  int index = elm_radio_value_get(picker->radioMain);
  elm_genlist_item_selected_set(callback_data->it, true);
  picker->selectedIndex = index;
  view_popup_menu_select(picker->web_view_, index);
}

static char* __picker_label_get_cb(void* data, Evas_Object* obj, const char* part) {
  genlist_callback_data *callback_data = static_cast<genlist_callback_data*>(data);
  Popup_Menu_Item* menuItem = static_cast<Popup_Menu_Item*>(callback_data->menuItem);

  if (!strncmp(part, "elm.text", strlen("elm.text")))
    return elm_entry_utf8_to_markup(popup_menu_item_text_get(menuItem));

  return 0;
}

static Evas_Object* __picker_icon_get_cb(void* data, Evas_Object* obj, const char* part) {
  genlist_callback_data *callback_data = static_cast<genlist_callback_data*>(data);
  Popup_Picker* picker = static_cast<Popup_Picker*>(callback_data->user_data);

  if (!strcmp(part, "elm.swallow.end") &&
      popup_menu_item_enabled_get(callback_data->menuItem) == EINA_TRUE) {
    Evas_Object *radio = elm_radio_add(obj);
    elm_radio_state_value_set(radio, callback_data->index);
    elm_radio_group_add(radio, picker->radioMain);

    evas_object_size_hint_weight_set(radio, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(radio, EVAS_HINT_FILL, EVAS_HINT_FILL);
    evas_object_propagate_events_set(radio, EINA_FALSE);

    elm_radio_value_set(picker->radioMain, picker->selectedIndex);
    evas_object_smart_callback_add(radio, "changed", __picker_radio_icon_changed_cb, (void *)data);
    return radio;
  }
  return 0;
}

static void __picker_item_selected_cb(void* data, Evas_Object* obj, void* event_info) {
  genlist_callback_data *callback_data = static_cast<genlist_callback_data*>(data);
  Popup_Picker* picker = static_cast<Popup_Picker*>(callback_data->user_data);
  Evas_Object* radio_icon = elm_object_item_part_content_get((Elm_Object_Item *)event_info, "elm.swallow.end");

  int index = callback_data->index;
  elm_genlist_item_selected_set(callback_data->it, false);

  if (popup_menu_item_enabled_get(callback_data->menuItem) == EINA_TRUE) {
    elm_radio_value_set(radio_icon, callback_data->index);
    picker->selectedIndex = index;
    view_popup_menu_select(picker->web_view_, index);
  }
}

void clear_genlist_callback_data(Popup_Picker* picker) {
  if (picker && picker->m_genlist_callback_data_list) {
    Eina_List* list;
    void* data;

    EINA_LIST_FOREACH(picker->m_genlist_callback_data_list, list, data) {
      if (data)
        free ((genlist_callback_data *)data);
    }
    eina_list_free(picker->m_genlist_callback_data_list);
    picker->m_genlist_callback_data_list = NULL;
  }
}

void menuItemActivated(void* data, Evas_Object* obj, void* event_info) {
  Popup_Picker* picker = static_cast<Popup_Picker*>(data);
  Elm_Object_Item* selected = static_cast<Elm_Object_Item*>(event_info);
  int index = elm_genlist_item_index_get(selected);
  // elm_genlist_item_index_get iterrate starting from 1
  if (index > 0)
    index--;

  if (picker->multiSelect) {
    int pos = eina_inarray_search(picker->changedList, &index, compareChangedItems);
    if (pos == -1)
      eina_inarray_push(picker->changedList, &index);
    else
      eina_inarray_remove(picker->changedList, &index);
  }

  view_popup_menu_multiple_select(picker->web_view_, picker->changedList);
}

void menuItemDeactivated(void* data, Evas_Object* obj, void* event_info) {
  Popup_Picker* picker = static_cast<Popup_Picker*>(data);
  Elm_Object_Item* deselectedItem = static_cast<Elm_Object_Item*>(event_info);

  int deselectedIndex = elm_genlist_item_index_get(deselectedItem);
  // elm_genlist_item_index_get iterrate starting from 1
  if (deselectedIndex > 0)
    deselectedIndex--;
  int pos = eina_inarray_search(picker->changedList, &deselectedIndex, compareChangedItems);

  if (pos == -1)
    eina_inarray_push(picker->changedList, &deselectedIndex);
  else
    eina_inarray_remove(picker->changedList, &deselectedIndex);

  view_popup_menu_multiple_select(picker->web_view_, picker->changedList);
}

void listClosed(void* data, Evas_Object* obj, const char* emission, const char* source) {
  Popup_Picker* picker = static_cast<Popup_Picker*>(data);

  if (picker->multiSelect)
    view_popup_menu_multiple_select(picker->web_view_, picker->changedList);
  else
    view_popup_menu_select(picker->web_view_, picker->selectedIndex);

  if (picker->changedList) {
    eina_inarray_free(picker->changedList);
    picker->changedList = 0;
  }

  // FIXME: remove this check
  if (!picker->multiSelect)
    clear_genlist_callback_data(picker);

  view_popup_menu_close(picker->web_view_);
}

void navigateToNext(void *data, Evas_Object *obj, const char *emission, const char *source)
{
  //Popup_Picker* picker = static_cast<Popup_Picker*>(data);
  // DJKim : FIXME
  //ewk_view_form_navigate(picker->parent, true);
}

void navigateToPrev(void *data, Evas_Object *obj, const char *emission, const char *source)
{
  //Popup_Picker* picker = static_cast<Popup_Picker*>(data);
  // DJKim : FIXME
  //ewk_view_form_navigate(picker->parent, false);
}

static char* _listLabelGet(void* data, Evas_Object* obj, const char* part)
{
  Popup_Menu_Item* menuItem = static_cast<Popup_Menu_Item*>(data);

  if (!strncmp(part, "elm.text", strlen("elm.text")) && popup_menu_item_text_get(menuItem))
    return elm_entry_utf8_to_markup(popup_menu_item_text_get(menuItem));
  return 0;
}

#ifdef OS_TIZEN_MOBILE
static void popupMenuHwBackKeyCallback(void* data, Evas_Object* obj, void* eventInfo) {
  listClosed(data, obj, 0, 0);
}
#endif

static void createAndShowPopupList(Evas_Object* win, Popup_Picker* picker, Eina_List* items, int selectedIndex) {
  picker->selectedIndex = selectedIndex;
  picker->popupList = elm_genlist_add(win);
  evas_object_size_hint_weight_set(picker->popupList, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(picker->popupList, EVAS_HINT_FILL, EVAS_HINT_FILL);

  if (picker->multiSelect)
  {
    eina_inarray_flush(picker->changedList);
    elm_genlist_multi_select_set(picker->popupList, true);
  }
  static Elm_Genlist_Item_Class itemClass;
  void* itemv;
  const Eina_List* l;
  picker->firstItem = 0;

  picker->m_genlist_callback_data_list = NULL;

  // If it is multiselect, use the old implementation.
  //FIXME: remove this check, once picker with buttons is implemented for multiselect.
  if (!picker->multiSelect) {
    picker->radioMain = elm_radio_add(picker->popupList);
    if (!picker->radioMain) {
      LOG(ERROR) << "elm_radio_add failed. ";
      return;
    }

    elm_radio_state_value_set(picker->radioMain, 0);
    elm_radio_value_set(picker->radioMain, 0);

    itemClass.item_style = "default";
    itemClass.func.text_get = __picker_label_get_cb;
    itemClass.func.content_get = __picker_icon_get_cb;
    itemClass.func.state_get = 0;
    itemClass.func.del = 0;

    int index = 0;
    Eina_Bool has_problem = EINA_FALSE;
    EINA_LIST_FOREACH(items, l, itemv) {
      genlist_callback_data *data = (genlist_callback_data *)malloc(sizeof(genlist_callback_data));
      if (!data) {
        has_problem = EINA_TRUE;
        break;
      }

      memset(data, 0x00, sizeof(genlist_callback_data));
      data->index = index;
      data->user_data = picker;
      data->menuItem = static_cast<Popup_Menu_Item*>(itemv);
      data->it =  elm_genlist_item_append(picker->popupList, &itemClass, data, NULL, ELM_GENLIST_ITEM_NONE, __picker_item_selected_cb, data);
      picker->m_genlist_callback_data_list = eina_list_append(picker->m_genlist_callback_data_list, data);

      if (!index)
        picker->firstItem = data->it;

      if (selectedIndex == index) {
        elm_radio_value_set(picker->radioMain, index);
        elm_genlist_item_bring_in(data->it, ELM_GENLIST_ITEM_SCROLLTO_IN);
      }
      index++;
    }

    if (has_problem == EINA_TRUE) {
      clear_genlist_callback_data(picker);
      return;
    }
    evas_object_smart_callback_add(picker->popupList, "changed", __picker_radio_icon_changed_cb, picker);
  } else {
    itemClass.item_style = "default";
    itemClass.func.text_get = _listLabelGet;
    itemClass.func.content_get = 0;
    itemClass.func.state_get = 0;
    itemClass.func.del = 0;

    void* itemv;
    const Eina_List* l;

    picker->firstItem = 0;
    int index = 0;
    EINA_LIST_FOREACH(items, l, itemv) {
      Popup_Menu_Item* menuItem = static_cast<Popup_Menu_Item*>(itemv);
      Elm_Object_Item* itemObject = elm_genlist_item_append(picker->popupList, &itemClass, menuItem, 0, ELM_GENLIST_ITEM_NONE, 0, 0);
      if (!index)
        picker->firstItem = itemObject;

      if (popup_menu_item_selected_get(menuItem)) {
        eina_inarray_push(picker->changedList, &index);
        elm_genlist_item_selected_set(itemObject, EINA_TRUE);
      }
      if (!(popup_menu_item_enabled_get(menuItem)))
        elm_object_item_disabled_set(itemObject, EINA_TRUE);
      index++;
    }

    evas_object_smart_callback_add(picker->popupList, "selected", menuItemActivated, picker);

    if (picker->multiSelect)
      evas_object_smart_callback_add(picker->popupList, "unselected", menuItemDeactivated, picker);
  }

  elm_object_focus_allow_set(picker->popupList, false);
  evas_object_show(picker->popupList);

  Evas_Object *bgcolor = elm_bg_add(picker->container);
  elm_object_part_content_set(picker->container, "bg", bgcolor);
  elm_object_part_content_set(picker->container, "elm.swallow.content", picker->popupList);

  // DJKim : FIXME
  edje_object_part_text_set(elm_layout_edje_get(picker->container), "elm.text.done", "Done");
}

static void resizeAndShowPicker(Popup_Picker* picker) {
  /* resize picker to window */
  Eina_Rectangle windowRect;
  Ecore_Evas* ee = ecore_evas_ecore_evas_get(evas_object_evas_get(picker->container));
  ecore_evas_geometry_get(ee, &windowRect.x, &windowRect.y, &windowRect.w, &windowRect.h);

  evas_object_resize(picker->container, windowRect.w, windowRect.h);
  evas_object_move(picker->container, windowRect.x, windowRect.y);
  evas_object_show(picker->container);
}

Popup_Picker* popup_picker_new(EWebView* web_view, Evas_Object* parent, Eina_List* items, int selectedIndex, Eina_Bool multiple) {
  Popup_Picker* picker = new Popup_Picker;
  picker->parent = parent;
  picker->web_view_ = web_view;
  picker->multiSelect = multiple;
  picker->changedList = eina_inarray_new(sizeof(int), 0);

  Evas_Object* obj = elm_object_parent_widget_get(parent);
  if (obj == NULL)
    obj = parent;
  Evas_Object* topParent = elm_object_top_widget_get(obj);
  picker->win = elm_win_add(topParent, "combo_box Window", ELM_WIN_BASIC);
  elm_win_alpha_set(picker->win, EINA_TRUE);

#ifdef OS_TIZEN
  if (elm_win_wm_rotation_supported_get(topParent)) {
    int rots[4] = {0, 90, 180, 270};
    elm_win_wm_rotation_available_rotations_set(picker->win, rots, 4);
  }
#endif

  int width = 0;
  int height = 0;
  ecore_x_screen_size_get(ecore_x_default_screen_get(), &width, &height);
  evas_object_resize(picker->win, width, height);
  evas_object_move(picker->win, 0, 0);
  evas_object_show(picker->win);

  picker->container = elm_layout_add(picker->win);
  evas_object_size_hint_weight_set(picker->container, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  elm_win_resize_object_add(picker->win, picker->container);

  base::FilePath edj_dir;
  base::FilePath control_edj;
  PathService::Get(PathsEfl::EDJE_RESOURCE_DIR, &edj_dir);
  control_edj = edj_dir.Append(FILE_PATH_LITERAL("control.edj"));

  if (!elm_layout_file_set(picker->container, control_edj.AsUTF8Unsafe().c_str(), "elm/picker"))
    LOG(ERROR) << "error elm_layout_file_set, control_edj " ;
  else {
    edje_object_signal_callback_add(elm_layout_edje_get(picker->container),
      "mouse,clicked,1", "elm.image.done_bg", listClosed, picker);
    edje_object_signal_callback_add(elm_layout_edje_get(picker->container),
      "mouse,clicked,1", "dimm", listClosed, picker);
    edje_object_signal_callback_add(elm_layout_edje_get(picker->container),
      "mouse,clicked,1", "elm.image.prev_bg", navigateToPrev, picker);
    edje_object_signal_callback_add(elm_layout_edje_get(picker->container),
      "mouse,clicked,1", "elm.image.next_bg", navigateToNext, picker);
  }

#ifdef OS_TIZEN_MOBILE
  if (EflAssistHandle) {
    void (*webkit_ea_object_event_callback_add)(Evas_Object *, Ea_Callback_Type , Ea_Event_Cb func, void *);
    webkit_ea_object_event_callback_add = (void (*)(Evas_Object *, Ea_Callback_Type , Ea_Event_Cb func, void *))dlsym(EflAssistHandle, "ea_object_event_callback_add");
    (*webkit_ea_object_event_callback_add)(picker->container, EA_CALLBACK_BACK, popupMenuHwBackKeyCallback, picker);
  }
#endif

  createAndShowPopupList(picker->win, picker, items, selectedIndex);
  evas_object_propagate_events_set(picker->container, false);

  resizeAndShowPicker(picker);
  edje_object_signal_emit(elm_layout_edje_get(picker->container), "show,picker,signal", "");
  edje_object_signal_emit(elm_layout_edje_get(picker->container), "show,prev_button,signal", "");
  edje_object_signal_emit(elm_layout_edje_get(picker->container), "show,next_button,signal", "");

  return picker;
}

void popup_picker_resize(Popup_Picker* picker) {
  resizeAndShowPicker(picker);
}

void popup_picker_del(Popup_Picker* picker) {
  if (!picker->multiSelect) {
    evas_object_smart_callback_del(picker->popupList, "changed", __picker_radio_icon_changed_cb);
    clear_genlist_callback_data(picker);
  }
  evas_object_smart_callback_del(picker->popupList, "selected", menuItemActivated);
  evas_object_del(picker->popupList);
  evas_object_del(picker->container);
  evas_object_del(picker->win);

  picker->popupList = 0;
  picker->firstItem = 0;
  picker->web_view_ = 0;

  delete picker;
}

void popup_picker_update(Evas_Object* parent, Popup_Picker* picker, Eina_List* items, int selectedIndex) {
  // FIXME: A check should be made if the items are changed instead of directly changing them.
  // Another issue is that if the list is scrolled, the old scroll position is not retained as
  // the list is replaced directly.
  evas_object_del(picker->popupList);

  picker->changedList = eina_inarray_new(sizeof(int), 0);
  createAndShowPopupList(picker->win, picker, items, selectedIndex);
}

void popup_picker_buttons_update(Popup_Picker* picker, int position, int count, bool enable) {
  Evas_Object* layoutObj = elm_layout_edje_get(picker->container);
  if (enable) {
    if (position == 0) {
      edje_object_signal_emit(layoutObj, "enable,prev_button,signal", "");
      edje_object_signal_callback_add(elm_layout_edje_get(picker->container),
        "mouse,clicked,1", "elm.image.prev_bg", navigateToPrev, picker);
    }
    if (position == count - 1) {
      edje_object_signal_emit(layoutObj, "enable,next_button,signal", "");
      edje_object_signal_callback_add(elm_layout_edje_get(picker->container),
        "mouse,clicked,1", "elm.image.next_bg", navigateToNext, picker);
    }
  } else {
    if (position == 0) {
      edje_object_signal_emit(layoutObj, "disable,prev_button,signal", "");
      edje_object_signal_callback_del(elm_layout_edje_get(picker->container),
        "mouse,clicked,1", "elm.image.prev_bg", navigateToPrev);
    }
    if (position == count - 1) {
      edje_object_signal_emit(layoutObj, "disable,next_button,signal", "");
      edje_object_signal_callback_del(elm_layout_edje_get(picker->container),
        "mouse,clicked,1", "elm.image.next_bg", navigateToNext);
    }
  }
}

void view_popup_menu_select(EWebView* web_view, int selectedIndex) {
  web_view->DidSelectPopupMenuItem(selectedIndex);
}

void view_popup_menu_multiple_select(EWebView* web_view, Eina_Inarray* changeList) {
  std::vector<int> selectedIndices;
  Eina_Iterator* itr;
  void* data;
  itr = eina_inarray_iterator_new(changeList);

  EINA_ITERATOR_FOREACH(itr, data) {
    int* pData = static_cast<int*>(data);
    selectedIndices.push_back(*pData);
  }
  eina_iterator_free(itr);

  web_view->DidMultipleSelectPopupMenuItem(selectedIndices);
}

void view_popup_menu_close(EWebView* web_view) {
  web_view->PopupMenuClose();
}
