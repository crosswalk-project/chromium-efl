// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <Elementary.h>
#include <stdio.h>
#include <stdlib.h>

#include "API/ewk_context_menu_private.h"
#include "base/files/file_path.h"
#include "components/clipboard/clipboard_helper_efl.h"
#include "context_menu_controller_efl.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/download_manager.h"
#include "content/public/browser/navigation_entry.h"
#include "eweb_view.h"
#include "net/base/net_util.h"
#include "net/base/filename_util.h"
#include "third_party/WebKit/public/platform/WebReferrerPolicy.h"
#include "ui/base/clipboard/clipboard.h"

#include "tizen_webview/public/tw_webview.h"
#include "tizen_webview/public/tw_selection_controller.h"

// TODO: remove this header dependency
#include "selection_controller_efl.h"

#if defined(OS_TIZEN_MOBILE)
#include <efl_assist.h>
#endif

using tizen_webview::WebView;
using tizen_webview::SelectionController;

namespace {
static const int kMaxHeightVertical = 680;
static const int kMaxHeightHorizontal = 360;
}

namespace content {

int ContextMenuControllerEfl::_appended_item_size = 0;
int ContextMenuControllerEfl::_popup_item_height = 96;
bool ContextMenuControllerEfl::_context_menu_resized = false;
std::vector<ContextMenuItemEfl> ContextMenuControllerEfl::_context_menu_listdata;

ContextMenuControllerEfl::~ContextMenuControllerEfl() {
  for (std::set<DownloadItem*>::iterator it = observed_download_items_.begin();
       it != observed_download_items_.end(); ++it) {
    (*it)->RemoveObserver(this);
  }

  _context_menu_listdata.clear();
  DeletePopup();
}

bool ContextMenuControllerEfl::PopulateAndShowContextMenu(const ContextMenuParams &params) {
  if (!webview_)
    return false;

  EWebView* view = webview_->GetImpl();
  if (!view)
    return false;

  DCHECK(menu_items_ == NULL);
  if (menu_items_)
    return false;

  params_ = params;
  GetProposedContextMenu();
   _Ewk_Context_Menu new_menu(menu_items_);
   view->SmartCallback<EWebViewCallbacks::ContextMenuCustomize>().call(&new_menu);
   menu_items_ = new_menu.TakeMenuList();

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
  ContextMenuItemEfl* new_menu_item = new ContextMenuItemEfl(
      item, option, title, image_url, link_url, icon_path);
  _Ewk_Context_Menu_Item* new_item = new _Ewk_Context_Menu_Item(new_menu_item);
  menu_items_ = eina_list_append(menu_items_, new_item);
}

void ContextMenuControllerEfl::GetProposedContextMenu() {
  if (!params_.link_url.is_empty()) {
    AddItemToPropsedList(MENU_ITEM_TYPE_ACTION,
        MENU_ITEM_OPEN_LINK_IN_NEW_WINDOW,
        std::string(dgettext("WebKit",
            "IDS_WEBVIEW_OPT_OPEN_LINK_IN_NEW_TAB_ABB")),
        params_.link_url.spec(),
        params_.link_url.spec());
    AddItemToPropsedList(MENU_ITEM_TYPE_ACTION,
        MENU_ITEM_COPY_LINK_TO_CLIPBOARD,
        std::string(dgettext("WebKit", "IDS_WEBVIEW_OPT_COPY_LINK_URL_ABB")),
        std::string(),
        params_.link_url.spec());
    AddItemToPropsedList(MENU_ITEM_TYPE_ACTION,
        MENU_ITEM_DOWNLOAD_LINK_TO_DISK,
        std::string(dgettext("WebKit", "IDS_BR_BODY_SAVE_LINK")),
        std::string(),
        params_.link_url.spec());
  }
  if (params_.is_editable && ClipboardHelperEfl::NumberOfItems() > 0) {
    AddItemToPropsedList(MENU_ITEM_TYPE_ACTION, MENU_ITEM_PASTE,
        std::string(dgettext("WebKit", "IDS_WEBVIEW_OPT_PASTE")));
  }
  if (!params_.selection_text.empty() && params_.is_editable) {
    AddItemToPropsedList(MENU_ITEM_TYPE_ACTION, MENU_ITEM_CUT,
        std::string(dgettext("WebKit", "IDS_WEBVIEW_OPT_CUT_ABB")));
  }
  if (!params_.selection_text.empty()) {
    AddItemToPropsedList(MENU_ITEM_TYPE_ACTION, MENU_ITEM_COPY,
        std::string(dgettext("WebKit", "IDS_WEBVIEW_OPT_COPY")));
    if (!params_.is_editable) {
      AddItemToPropsedList(MENU_ITEM_TYPE_ACTION, MENU_ITEM_SEARCH_WEB,
          std::string());
      AddItemToPropsedList(MENU_ITEM_TYPE_ACTION, MENU_ITEM_SEARCH_IN_SPOTLIGHT,
          std::string());
    }
  }

 if (params_.has_image_contents && !params_.src_url.is_empty()) {
    AddItemToPropsedList(MENU_ITEM_TYPE_ACTION,
        MENU_ITEM_OPEN_IMAGE_IN_NEW_WINDOW,
        std::string(dgettext("WebKit",
            "IDS_WEBVIEW_OPT_OPEN_IMAGE_IN_NEW_TAB_ABB")),
        params_.src_url.spec(),
        params_.src_url.spec());
    AddItemToPropsedList(MENU_ITEM_TYPE_ACTION,
        MENU_ITEM_DOWNLOAD_IMAGE_TO_DISK,
        std::string(dgettext("WebKit", "IDS_WEBVIEW_OPT_SAVE_IMAGE_ABB")),
        params_.src_url.spec(),
        params_.src_url.spec());
    AddItemToPropsedList(MENU_ITEM_TYPE_ACTION,
        MENU_ITEM_COPY_IMAGE_TO_CLIPBOARD,
        std::string(dgettext("WebKit", "IDS_WEBVIEW_OPT_COPY_IMAGE")),
        params_.src_url.spec(),
        params_.src_url.spec());
  } else if (!params_.link_url.is_empty()) {
    AddItemToPropsedList(MENU_ITEM_TYPE_ACTION,
        MENU_ITEM_TEXT_SELECTION_MODE,
        std::string(dgettext("WebKit", "IDS_WEBVIEW_OPT_SELECTION_MODE_ABB")),
        params_.link_url.spec(),
        params_.link_url.spec());
  }
  if (!params_.selection_text.empty()
      || (params_.is_editable && !webview_->GetImpl()->IsLastAvailableTextEmpty())) {
    AddItemToPropsedList(MENU_ITEM_TYPE_ACTION, MENU_ITEM_SELECT_WORD,
        std::string(dgettext("WebKit", "IDS_WEBVIEW_OPT_SELECT_ABB")));
    AddItemToPropsedList(MENU_ITEM_TYPE_ACTION, MENU_ITEM_SELECT_ALL,
        std::string(dgettext("WebKit", "IDS_WEBVIEW_OPT_SELECT_ALL_ABB")));
  }
#if !defined(EWK_BRINGUP)
  if (params_.is_draggable) {
    AddItemToPropsedList(MENU_ITEM_TYPE_ACTION, MENU_ITEM_DRAG,
        std::string(dgettext("WebKit", "IDS_WEBVIEW_OPT_DRAG_AND_DROP")));
  }
#endif
  if (params_.is_editable && ClipboardHelperEfl::NumberOfItems() > 0) {
    AddItemToPropsedList(MENU_ITEM_TYPE_ACTION, MENU_ITEM_CLIPBOARD,
        std::string(dgettext("WebKit", "IDS_WEBVIEW_OPT_CLIPBOARD")));
  }
}

bool ContextMenuControllerEfl::CreateContextMenu() {
  is_text_selection_ = false;
  Eina_List* ittr;
  void* data;
  EINA_LIST_FOREACH(menu_items_, ittr, data) {
    _Ewk_Context_Menu_Item* item = static_cast<_Ewk_Context_Menu_Item*> (data);
    ContextMenuItemEfl* context_item = item->GetMenuItem();
    switch (context_item->GetContextMenuOption()) {
      case MENU_ITEM_COPY:
      case MENU_ITEM_SELECT_ALL:
      case MENU_ITEM_SELECT_WORD:
      case MENU_ITEM_PASTE:
        is_text_selection_ = true;
        break;
      default:
        break;
    }
  }

  top_widget_ = elm_object_parent_widget_get(GetWebViewEvasObject());
  if (!is_text_selection_)
    top_widget_ = elm_object_top_widget_get(elm_object_parent_widget_get(GetWebViewEvasObject()));

  if (!top_widget_)
    top_widget_ = GetWebViewEvasObject();

  if (is_text_selection_) {
    popup_ = elm_ctxpopup_add(top_widget_);
    elm_object_style_set(popup_, "copypaste");
    elm_ctxpopup_horizontal_set(popup_, EINA_TRUE);
    elm_object_tree_focus_allow_set(popup_, false);
  } else {
    evas_object_data_set(top_widget_, "ContextMenuContollerEfl", this);
    widget_win_ = elm_win_add(top_widget_, "Blink Center Popup", ELM_WIN_BASIC);
    if (!widget_win_)
      return false;

    elm_win_alpha_set(widget_win_, EINA_TRUE);
    ecore_x_icccm_name_class_set(elm_win_xwindow_get(widget_win_), "APP_POPUP", "APP_POPUP");
#if defined(OS_TIZEN)
    if (elm_win_wm_rotation_supported_get(top_widget_)) {
      int preferred_rotation = elm_win_wm_rotation_preferred_rotation_get(top_widget_);
      if (preferred_rotation == -1) {
        int rots[4] = {0, 90, 180, 270};
        elm_win_wm_rotation_available_rotations_set(widget_win_, rots, 4);
      } else {
        elm_win_wm_rotation_available_rotations_set(widget_win_, &preferred_rotation, 1);
      }
    }
#endif
    popup_ = elm_popup_add(widget_win_);
  }

  if (!popup_)
    return false;

  evas_object_data_set(popup_, "ContextMenuContollerEfl", this);

  _appended_item_size = 0;
  if (is_text_selection_) {
    EINA_LIST_FOREACH(menu_items_, ittr, data) {
      _Ewk_Context_Menu_Item* item = static_cast<_Ewk_Context_Menu_Item*> (data);
      ContextMenuItemEfl* context_item = item->GetMenuItem();
      Elm_Object_Item* appended_item = 0;

      if (!context_item->Title().empty()) {
        appended_item = elm_ctxpopup_item_append(popup_,
            context_item->Title().c_str(), 0, ContextMenuItemSelectedCallback,
            context_item);
      } else {
        appended_item = elm_ctxpopup_item_append(popup_, 0, 0,
            ContextMenuItemSelectedCallback,
            context_item);
      }

      if (appended_item)
        _appended_item_size++;
    }
  } else {
    GURL url = web_contents_.GetVisibleURL();
    std::string selected_link (url.host().c_str());
    if (!selected_link.empty()) {
      if (StartsWithASCII(selected_link, std::string("mailto:"), false)
          || StartsWithASCII(selected_link, std::string("tel:"), false)) {
        std::vector<std::string> link_split;
        size_t current_pos = 0;
        size_t next_delimiter = 0;
        while ((next_delimiter = selected_link.find(":", current_pos))
               != std::string::npos) {
          std::string part = selected_link.substr(current_pos,
                                                  next_delimiter - current_pos);
          link_split.push_back(part);
          current_pos = next_delimiter + 1;
        }
        std::string last_part = selected_link.substr(current_pos);
        if (!last_part.empty())
          link_split.push_back(last_part);

        if (!link_split[1].size()) {
          std::vector<std::string> real_link_split;
          size_t current_pos = 0;
          size_t next_delimiter = 0;
          while ((next_delimiter = link_split[1].find("?", current_pos)) != std::string::npos) {
            std::string part = link_split[1].substr(current_pos, next_delimiter - current_pos);
            real_link_split.push_back(part);
            current_pos = next_delimiter + 1;
          }
          std::string last_part = link_split[1].substr(current_pos);
          if (!last_part.empty())
            real_link_split.push_back(last_part);

          elm_object_part_text_set(popup_, "title,text", real_link_split[0].c_str());
        }
      } else {
        elm_object_part_text_set(popup_, "title,text", selected_link.c_str());
      }
    } else {
      std::string selected_image (url.host().c_str());
      if (!selected_image.empty()) {
        if (StartsWithASCII(selected_image, std::string("data:"), false))
          elm_object_part_text_set(popup_, "title,text", "data:");
        else
          elm_object_part_text_set(popup_, "title,text", selected_image.c_str());
      }
    }

    int width = 0;
    int height = 0;
    ecore_x_screen_size_get(ecore_x_default_screen_get(), &width, &height);

    evas_object_size_hint_weight_set(popup_, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_resize(widget_win_, width, height);

    static Elm_Genlist_Item_Class _context_menu_list_item;

    _context_menu_list_item.item_style = "default";
    _context_menu_list_item.func.text_get = ContextMenuGenlistTextSet;
    _context_menu_list_item.func.content_get = 0;
    _context_menu_list_item.func.state_get = 0;
    _context_menu_list_item.func.del = 0;

    Evas_Object* genlist = elm_genlist_add(popup_);
    evas_object_data_set(genlist, "ContextMenuContollerEfl", this);
    evas_object_size_hint_align_set(genlist, EVAS_HINT_FILL, EVAS_HINT_FILL);
    evas_object_smart_callback_add(genlist, "realized", ContextMenuListRealized, 0);
    elm_scroller_content_min_limit(genlist, EINA_FALSE, EINA_TRUE);

    _context_menu_listdata.clear();

    Eina_List* ittr;
    void* data;
    EINA_LIST_FOREACH(menu_items_, ittr, data) {
      _Ewk_Context_Menu_Item* item = static_cast<_Ewk_Context_Menu_Item*> (data);
      ContextMenuItemEfl* context_item = item->GetMenuItem();
      if (!context_item->Title().empty()) {
        Elm_Object_Item* appended_item = 0;
        appended_item = elm_genlist_item_append(genlist,
            &_context_menu_list_item,
            reinterpret_cast<const void*>(_appended_item_size),
            0,
            ELM_GENLIST_ITEM_NONE, ContextMenuItemSelectedCallback,
            context_item);

        if (appended_item) {
          _context_menu_listdata.push_back(*context_item);
          _appended_item_size++;
        }
      }
    }
    evas_object_show(genlist);
    elm_object_content_set(popup_, genlist);
    evas_object_event_callback_add(popup_, EVAS_CALLBACK_RESIZE,
        ContextMenuPopupResize, genlist);
    evas_object_smart_callback_add(widget_win_, "wm,rotation,changed",
        ContextMenuPopupRotationCallback, genlist);
  }

  if (!_appended_item_size) {
    DeletePopup();
    return false;
  }
  return true;
}

void ContextMenuControllerEfl::ContextMenuHWBackKey(void* data, Evas_Object* obj,
                                                    void* event_info) {
  ContextMenuControllerEfl* menu_controller = static_cast<ContextMenuControllerEfl*>(data);
  if (menu_controller) {
    menu_controller->HideContextMenu();
    evas_object_data_del(obj, "ContextEfl");
  }
}

void ContextMenuControllerEfl::ContextMenuCancelCallback(void* data, Evas_Object* obj,
                                                         void* event_info) {
  ContextMenuControllerEfl* menu_controller = static_cast<ContextMenuControllerEfl*>(evas_object_data_get(obj, "ContextEfl"));
  if (menu_controller) {
    menu_controller->HideContextMenu();
    menu_controller->HideSelectionHandle();
    evas_object_data_del(obj, "ContextEfl");
  }
}

void ContextMenuControllerEfl::ContextMenuItemSelectedCallback(void* data,
    Evas_Object* obj, void* event_info) {
  Evas_Object* pop_up = obj;
#if !defined(OS_TIZEN)
  // In desktop evas_object_data_get(obj) is not working because obj is not the object to which data was attached.
  // So get the parent, obj parent is internal box, box parent is the popup to which object was attached.
  // On target the obj that is obtained is the popup so the data is proper. Need to find why this difference?
  // Temp added this patch so as to work both on target and device.
  Evas_Object* list = elm_object_parent_widget_get(obj);
  pop_up = elm_object_parent_widget_get(list);
#endif
  ContextMenuControllerEfl* menu_controller =
      static_cast<ContextMenuControllerEfl*>
      (evas_object_data_get(pop_up, "ContextMenuContollerEfl"));

  if (menu_controller) {
    ContextMenuItemEfl* selected_menu_item = static_cast<ContextMenuItemEfl*>(data);
    if (selected_menu_item)
       menu_controller->MenuItemSelected(selected_menu_item);
    menu_controller->HideContextMenu();
  }
}

bool ContextMenuControllerEfl::ShowContextMenu() {
  if (!popup_)
    return false;

  if (is_text_selection_) {
    int webview_x, webview_y, webview_width, webview_height;
    Evas_Point point;

    gfx::Point popup_position = gfx::Point(params_.x, params_.y);

    if (!webview_)
      return false;

    Evas_Object* wv_evas_obj = GetWebViewEvasObject();
    evas_object_geometry_get(wv_evas_obj, &webview_x, &webview_y, &webview_width, &webview_height);
    popup_position.set_x(popup_position.x() + webview_x);
    popup_position.set_y(popup_position.y() + webview_y);

    point.x = popup_position.x();
    point.y = popup_position.y();

    evas_object_smart_callback_call(wv_evas_obj, "contextmenu,willshow", &point);

    bool allowed = true;
    evas_object_smart_callback_call(wv_evas_obj, "contextmenu,allowed", &allowed);
    if (!allowed) {
      DeletePopup();
      return false;
    }

    SelectionController* controller = webview_->GetSelectionController();
    if (!controller) {
      DeletePopup();
      return false;
    }

    int draw_direction;
    CastToSelectionControllerEfl(controller)->ChangeContextMenuPosition(popup_position,
                                                                        draw_direction);
    switch(draw_direction) {
      case 0:
        elm_ctxpopup_direction_priority_set(popup_,
                                            ELM_CTXPOPUP_DIRECTION_DOWN,
                                            ELM_CTXPOPUP_DIRECTION_DOWN,
                                            ELM_CTXPOPUP_DIRECTION_DOWN,
                                            ELM_CTXPOPUP_DIRECTION_DOWN);
        break;
      case 1:
        elm_ctxpopup_direction_priority_set(popup_,
                                            ELM_CTXPOPUP_DIRECTION_UP,
                                            ELM_CTXPOPUP_DIRECTION_UP,
                                            ELM_CTXPOPUP_DIRECTION_UP,
                                            ELM_CTXPOPUP_DIRECTION_UP);
        break;
      case 2:
        elm_ctxpopup_direction_priority_set(popup_,
                                            ELM_CTXPOPUP_DIRECTION_LEFT,
                                            ELM_CTXPOPUP_DIRECTION_LEFT,
                                            ELM_CTXPOPUP_DIRECTION_LEFT,
                                            ELM_CTXPOPUP_DIRECTION_LEFT);
        break;
      case 3:
        elm_ctxpopup_direction_priority_set(popup_,
                                            ELM_CTXPOPUP_DIRECTION_RIGHT,
                                            ELM_CTXPOPUP_DIRECTION_RIGHT,
                                            ELM_CTXPOPUP_DIRECTION_RIGHT,
                                            ELM_CTXPOPUP_DIRECTION_RIGHT);
        break;
      default:
        elm_ctxpopup_direction_priority_set(popup_,
                                            ELM_CTXPOPUP_DIRECTION_UP,
                                            ELM_CTXPOPUP_DIRECTION_DOWN,
                                            ELM_CTXPOPUP_DIRECTION_LEFT,
                                            ELM_CTXPOPUP_DIRECTION_RIGHT);
    };
    evas_object_move(popup_, popup_position.x(), popup_position.y());
    evas_object_smart_callback_add(popup_, "dismissed", ContextMenuCancelCallback, this);
#if defined(OS_TIZEN_MOBILE)
    elm_ctxpopup_auto_hide_disabled_set(popup_, EINA_TRUE);
#endif
    evas_object_focus_set(GetWebViewEvasObject(), EINA_TRUE);
  } else {
#if defined(OS_TIZEN_MOBILE)
    ea_object_event_callback_add(popup_, EA_CALLBACK_BACK, ContextMenuHWBackKey, this);
    evas_object_smart_callback_add(popup_, "unfocused", ContextMenuHWBackKey, this);
    evas_object_focus_set(popup_, EINA_TRUE);
#endif
    evas_object_show(widget_win_);
    evas_object_smart_callback_add(popup_, "block,clicked", BlockClickedCallback, this);
  }
  evas_object_show(popup_);
  return true;
}

void ContextMenuControllerEfl::HideSelectionHandle() {
  SelectionController* controller = webview_->GetSelectionController();
  if (controller)
    controller->HideHandle();
}

void ContextMenuControllerEfl::ShowSelectionHandleAndContextMenu() {
  SelectionController* controller = webview_->GetSelectionController();
  if (controller) {
    SelectionControllerEfl* controller_efl = CastToSelectionControllerEfl(controller);
    controller_efl->SetSelectionStatus(true);
    controller_efl->UpdateSelectionDataAndShow(
        controller_efl->GetLeftRect(),
        controller_efl->GetRightRect(),
        false /* unused */,
        true);
  }
}

void ContextMenuControllerEfl::OnDownloadStarted(
    content::DownloadItem* item,
    content::DownloadInterruptReason interrupt_reason) {
  item->AddObserver(this);
  observed_download_items_.insert(item);
}

void ContextMenuControllerEfl::OnDownloadUpdated(content::DownloadItem* download) {
  if(download && download->AllDataSaved()) {
    const std::string& DownloadPath = download->GetForcedFilePath().value();
    ClipboardHelperEfl::GetInstance()->SetData(DownloadPath,
                                               ClipboardHelperEfl::CLIPBOARD_DATA_TYPE_IMAGE);
    download->RemoveObserver(this);
    observed_download_items_.erase(download);
  }
}

base::FilePath ContextMenuControllerEfl::DownloadFile(const GURL url,
    const base::FilePath outputDir,
    const DownloadUrlParameters::OnStartedCallback &callback =
        DownloadUrlParameters::OnStartedCallback()) {
  LOG(INFO) << "Downloading file: " << url << "to: "<< outputDir.value();
  const GURL referrer = web_contents_.GetVisibleURL();
  DownloadManager* dlm = BrowserContext::GetDownloadManager(
      web_contents_.GetBrowserContext());

  scoped_ptr<DownloadUrlParameters> dl_params(
      DownloadUrlParameters::FromWebContents(&web_contents_, url));
  dl_params->set_post_id(-1);
  dl_params->set_referrer(
      content::Referrer(referrer, blink::WebReferrerPolicyAlways));
  dl_params->set_referrer_encoding("utf8");
  base::FilePath fileName = net::GenerateFileName(url,"","","","","");
  base::FilePath fullPath = outputDir.Append(fileName);

  while (PathExists(fullPath)) {
    unsigned int i;
    base::FilePath fileNameTmp;
    for (i = 0; PathExists(fullPath) && i <= 999; i++) {
      char buffer[6];
      snprintf(buffer, sizeof(buffer), "(%d)", i);
      fileNameTmp = fileName.InsertBeforeExtension(std::string(buffer));
      fullPath = outputDir.Append(fileNameTmp);
    }
  }

  dl_params->set_file_path(fullPath);
  dl_params->set_prompt(true);
  dl_params->set_callback(callback);
  dlm->DownloadUrl(dl_params.Pass());
  return fullPath;
}

bool ContextMenuControllerEfl::TriggerDownloadCb(const GURL url) {
  BrowserContextEfl* browser_context =
      static_cast<BrowserContextEfl*>(web_contents_.GetBrowserContext());
  if (browser_context) {
    EwkDidStartDownloadCallback* start_download_callback =
        browser_context->WebContext()->DidStartDownloadCallback();
    if (start_download_callback) {
      DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));
      start_download_callback->TriggerCallback(url.spec());
      return true;
    }
  }
  return false;
}

void ContextMenuControllerEfl::OpenInNewTab(const GURL url) {
  if (!url.is_valid())
    return;
  const GURL referrer = web_contents_.GetVisibleURL();
  content::OpenURLParams params(url,
                                content::Referrer(referrer,
                                                  blink::WebReferrerPolicyAlways),
                                                  -1,
                                                  NEW_FOREGROUND_TAB,
                                                  ui::PAGE_TRANSITION_LINK,
                                                  false);
  web_contents_.GetDelegate()->OpenURLFromTab(&web_contents_, params);
}

void ContextMenuControllerEfl::ContextMenuListTrackResize(void* data, Evas* e,
                                                          Evas_Object* obj, void* event_info)
{
  int height;
  evas_object_geometry_get(obj, 0, 0, 0, &height);

  if (_popup_item_height == height)
    return;

  _popup_item_height = height;
  ContextMenuPopupBoxResize(elm_object_top_widget_get(static_cast<Evas_Object*>(data)), data);
}

void ContextMenuControllerEfl::ContextMenuListRealized(void* data,
                                                       Evas_Object* obj, void* event_info)
{
  if (!event_info)
    return;

  Elm_Object_Item* item = static_cast<Elm_Object_Item*>(event_info);
  size_t index = reinterpret_cast<size_t>(elm_object_item_data_get(item));

#if defined(OS_TIZEN)
  Evas_Object* track = elm_object_item_track(item);
  evas_object_event_callback_add(track, EVAS_CALLBACK_RESIZE, ContextMenuListTrackResize, obj);
#endif

  if (index == _appended_item_size - 1)
    elm_object_item_signal_emit(item, "elm,state,bottomline,hide", "");
}

char* ContextMenuControllerEfl::ContextMenuGenlistTextSet(void* data,
                                                          Evas_Object* obj, const char* part)
{
  size_t index = reinterpret_cast<size_t>(data);
  if (index >= _appended_item_size)
    return 0;

  if (_context_menu_listdata.at(index).Title().empty())
    return 0;

  return strdup(_context_menu_listdata.at(index).Title().c_str());
}

void ContextMenuControllerEfl::ContextMenuPopupBoxResize(Evas_Object* top_widget, void* data)
{
  _context_menu_resized = true;

  Evas_Object* box = elm_object_parent_widget_get(static_cast<Evas_Object*>(data));

  int rotation = elm_win_rotation_get(top_widget);
  if ((rotation == 90 || rotation == 270) && _appended_item_size > 3)
    evas_object_size_hint_min_set(box, 0, kMaxHeightHorizontal);
  else if ((rotation == 0 || rotation == 180) && _appended_item_size > 7)
    evas_object_size_hint_min_set(box, 0, kMaxHeightVertical);
  else
    evas_object_size_hint_min_set(box, 0, _appended_item_size * _popup_item_height);

  evas_object_show(box);
}

void ContextMenuControllerEfl::ContextMenuPopupResize(void* data,
                                                      Evas* e,
                                                      Evas_Object* obj,
                                                      void* event_info)
{
   if (!_context_menu_resized)
    ContextMenuPopupBoxResize(elm_object_top_widget_get(obj), data);
}

void ContextMenuControllerEfl::ContextMenuPopupRotationCallback(void* data,
                                                                Evas_Object* obj,
                                                                void* event_info)
{
  ContextMenuPopupBoxResize(obj, data);
}

void ContextMenuControllerEfl::BlockClickedCallback(void* data, Evas_Object*, void*)
{
  ContextMenuControllerEfl* menu_controller = static_cast<ContextMenuControllerEfl*>(data);

  if (!menu_controller)
    return;

  menu_controller->HideContextMenu();
}

void ContextMenuControllerEfl::CustomMenuItemSelected(ContextMenuItemEfl* menu_item) {
  _Ewk_Context_Menu_Item item(new ContextMenuItemEfl(menu_item->GetContextMenuOptionType(),
                                                     menu_item->GetContextMenuOption(),
                                                     menu_item->Title(),
                                                     params_.src_url.spec(),
                                                     params_.link_url.spec(),
                                                     std::string()));
  EWebView* view = webview_->GetImpl();
  view->SmartCallback<EWebViewCallbacks::ContextMenuItemSelected>().call(&item);
}

void ContextMenuControllerEfl::MenuItemSelected(ContextMenuItemEfl* menu_item)
{
  if (!menu_item)
    return;

  EWebView* view = webview_->GetImpl();
  if (!view)
    return;

  // FIXME: Add cases as and when required
  switch(menu_item->GetContextMenuOption())
  {
    case MENU_ITEM_OPEN_LINK_IN_NEW_WINDOW: {
      OpenInNewTab(GURL(params_.link_url.spec()));
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
        HideSelectionHandle();
        Eina_Rectangle left_rect, right_rect;
        view->GetSelectionRange(&left_rect, &right_rect);
        view->MoveCaret(gfx::Point(right_rect.x, right_rect.y));
      } else {
        view->ExecuteEditCommand("Unselect", NULL);
      }
      break;
    }
    case MENU_ITEM_TEXT_SELECTION_MODE: {
      Evas_Coord x, y;
      evas_object_geometry_get(view->evas_object(), &x, &y, 0, 0);
      view->SelectLinkText(gfx::Point(params_.x - x, params_.y - y));
      ShowSelectionHandleAndContextMenu();
      break;
    }
    case MENU_ITEM_COPY_IMAGE_TO_CLIPBOARD: {
      DownloadFile(GURL(params_.src_url.spec()),
                        base::FilePath("/tmp/"),
                        base::Bind(&ContextMenuControllerEfl::OnDownloadStarted,weak_ptr_factory_.GetWeakPtr()));
      break;
    }
    case MENU_ITEM_COPY_LINK_TO_CLIPBOARD: {
      ClipboardHelperEfl::GetInstance()->SetData(params_.link_url.spec(),
                                                 ClipboardHelperEfl::CLIPBOARD_DATA_TYPE_URL);
      break;
    }
    case MENU_ITEM_DOWNLOAD_LINK_TO_DISK: {
#if defined(OS_TIZEN_MOBILE)
      if (!TriggerDownloadCb(GURL(params_.link_url.spec())))
        DownloadFile(GURL(params_.link_url.spec()), base::FilePath("/opt/usr/media/Downloads/Others/"));
#else
      if (!TriggerDownloadCb(GURL(params_.link_url.spec())))
        DownloadFile(GURL(params_.link_url.spec()), base::FilePath("/tmp/"));
#endif
      break;
    }
    case MENU_ITEM_DOWNLOAD_IMAGE_TO_DISK: {
#if defined(OS_TIZEN_MOBILE)
      if (!TriggerDownloadCb(GURL(params_.src_url.spec())))
        DownloadFile(GURL(params_.src_url.spec()), base::FilePath("/opt/usr/media/Images/"));
#else
      if (!TriggerDownloadCb(GURL(params_.src_url.spec())))
        DownloadFile(GURL(params_.src_url.spec()), base::FilePath("/tmp/"));
#endif
      break;
    }
    case MENU_ITEM_OPEN_IMAGE_IN_NEW_WINDOW: {
      OpenInNewTab(GURL(params_.src_url.spec()));
      break;
    }
    case MENU_ITEM_SELECT_WORD: {
      view->ExecuteEditCommand("SelectWord", NULL);
      ShowSelectionHandleAndContextMenu();
      break;
    }
    case MENU_ITEM_SELECT_ALL: {
      view->ExecuteEditCommand("SelectAll", NULL);
      ShowSelectionHandleAndContextMenu();
      break;
    }
    case MENU_ITEM_PASTE: {
      view->ExecuteEditCommand("paste", NULL);
      HideSelectionHandle();
      break;
    }
    case MENU_ITEM_CUT: {
      view->ExecuteEditCommand("cut", NULL);
      break;
    }
    case MENU_ITEM_CLIPBOARD: {
      // TODO: set true for richly editable.
      // Paste operations are blocked by Editor if content is not richly editible.
      // Need to find if view has rich editor.
      ClipboardHelperEfl::GetInstance()->OpenClipboardWindow(view, true);
      break;
    }
#if !defined(EWK_BRINGUP)
    case MENU_ITEM_DRAG: {
      RenderViewHost* rvh = web_contents_.GetRenderViewHost();
      rvh->SendDragStartInformationToRenderer();
      context_menu_show_pos_.SetPoint(params_.x, params_.y);
      break;
    }
#endif
    default:
      CustomMenuItemSelected(menu_item);
  }
}

void ContextMenuControllerEfl::HideContextMenu() {
  if (popup_) {
    evas_object_del(popup_);
    popup_ = NULL;
  }

  if (widget_win_) {
    evas_object_hide(widget_win_);
    evas_object_smart_callback_del(widget_win_, "wm,rotation,changed",
                                   ContextMenuPopupRotationCallback);
  }
  _context_menu_resized = false;

  if (menu_items_) {
    void* data;
    EINA_LIST_FREE(menu_items_, data) {
      _Ewk_Context_Menu_Item *item = static_cast<_Ewk_Context_Menu_Item*> (data);
      delete item;
    }
    menu_items_ = NULL;
  }
}

Evas_Object* ContextMenuControllerEfl::GetWebViewEvasObject() {
  return webview_->AsEvasObject();
}

void ContextMenuControllerEfl::DeletePopup() {
  if(top_widget_)
    evas_object_data_set(top_widget_, "ContextMenuContollerEfl", 0);

  if (popup_) {
    evas_object_data_set(popup_, "ContextMenuContollerEfl", 0);
    evas_object_del(popup_);
    popup_ = 0;
  }

  if (widget_win_) {
    if (!is_text_selection_)
      evas_object_smart_callback_del(widget_win_, "wm,rotation,changed",
                                     ContextMenuPopupRotationCallback);
    evas_object_del(widget_win_);
    widget_win_ = 0;
  }

  if (menu_items_) {
    void* data;
    EINA_LIST_FREE(menu_items_, data) {
      _Ewk_Context_Menu_Item *item = static_cast<_Ewk_Context_Menu_Item*> (data);
      delete item;
    }
    menu_items_ = NULL;
  }
}
}
