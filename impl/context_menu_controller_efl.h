// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef context_menu_controller_h
#define context_menu_controller_h

#include <Evas.h>
#include <set>

#include "base/memory/weak_ptr.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/download_item.h"
#include "content/public/browser/download_url_parameters.h"
#include "content/public/common/context_menu_params.h"

#include "tizen_webview/public/tw_context_menu_controller.h"
#include "tizen_webview/public/tw_webview.h"

namespace content {

class WebContentsDelegateEfl;

enum ContextMenuOption{
  MENU_ITEM_NO_ACTION = 0,
  MENU_ITEM_OPEN_LINK_IN_NEW_WINDOW,
  MENU_ITEM_DOWNLOAD_LINK_TO_DISK,
  MENU_ITEM_COPY_LINK_TO_CLIPBOARD,
  MENU_ITEM_OPEN_IMAGE_IN_NEW_WINDOW,
  MENU_ITEM_DOWNLOAD_IMAGE_TO_DISK,
  MENU_ITEM_COPY_IMAGE_TO_CLIPBOARD,
  MENU_ITEM_OPEN_FRAME_IN_NEW_WINDOW,
  MENU_ITEM_COPY,
  MENU_ITEM_GO_BACK,
  MENU_ITEM_GO_FORWARD,
  MENU_ITEM_STOP,
  MENU_ITEM_RELOAD,
  MENU_ITEM_CUT,
  MENU_ITEM_PASTE,
  MENU_ITEM_SPELLING_GUESS,
  MENU_ITEM_NO_GUESSES_FOUND,
  MENU_ITEM_IGNORE_SPELLING,
  MENU_ITEM_LEARN_SPELLING,
  MENU_ITEM_OTHER,
  MENU_ITEM_SEARCH_IN_SPOTLIGHT,
  MENU_ITEM_SEARCH_WEB,
  MENU_ITEM_LOOK_UP_IN_DICTIONARY,
  MENU_ITEM_OPEN_WITH_DEFAULT_APPLICATION,
  MENU_ITEM_PDF_ACTUAL_SIZE,
  MENU_ITEM_PDF_ZOOM_IN,
  MENU_ITEM_PDF_ZOOM_OUT,
  MENU_ITEM_PDF_AUTO_SIZE,
  MENU_ITEM_PDF_SINGLE_PAGE,
  MENU_ITEM_PDF_FACTING_PAGES,
  MENU_ITEM_PDF_CONTINUOUS,
  MENU_ITEM_PDF_NEXT_PAGE,
  MENU_ITEM_PDF_PREVIOUS_PAGE,
  MENU_ITEM_OPEN_LINK,
  MENU_ITEM_IGNORE_GRAMMAR,
  MENU_ITEM_SPELLING_MENU,
  MENU_ITEM_SHOW_SPELLING_PANEL,
  MENU_ITEM_CHECK_SPELLING,
  MENU_ITEM_CHECK_SPELLING_WHILE_TYPING,
  MENU_ITEM_CHECK_GRAMMAR_WITH_SPELLING,
  MENU_ITEM_FONT_MENU,
  MENU_ITEM_SHOW_FONTS,
  MENU_ITEM_BOLD,
  MENU_ITEM_ITALIC,
  MENU_ITEM_UNDERLINE,
  MENU_ITEM_OUTLINE,
  MENU_ITEM_STYLES,
  MENU_ITEM_SHOW_COLORS,
  MENU_ITEM_SPEECH_MENU,
  MENU_ITEM_START_SPEAKING,
  MENU_ITEM_STOP_SPEAKING,
  MENU_ITEM_WRITING_DIRECTION_MENU,
  MENU_ITEM_DEFAULT_DIRECTION,
  MENU_ITEM_LEFT_TO_RIGHT,
  MENU_ITEM_RIGHT_TO_LEFT,
  MENU_ITEM_PDF_SINGLE_PAGE_SCROLLING,
  MENU_ITEM_PDF_FACING_PAGES_SCROLLING,
  MENU_ITEM_INSPECT_ELEMENT,
  MENU_ITEM_TEXT_DIRECTION_MENU,
  MENU_ITEM_TEXT_DIRECTION_DEFAULT,
  MENU_ITEM_TEXT_DIRECTION_LEFT_TO_RIGHT,
  MENU_ITEM_TEXT_DIRECTION_RIGHT_TO_LEFT,
  MENU_ITEM_CORRECT_SPELLING_AUTOMATICALLY,
  MENU_ITEM_SUBSTITUTIONS_MENU,
  MENU_ITEM_SHOW_SUBSTITUTIONS,
  MENU_ITEM_SMART_COPY_PASTE,
  MENU_ITEM_SMART_QUOTES,
  MENU_ITEM_SMART_DASHES,
  MENU_ITEM_SMART_LINKS,
  MENU_ITEM_TEXT_REPLACEMENT,
  MENU_ITEM_TRANSFORMATIONS_MENU,
  MENU_ITEM_MAKE_UPPER_CASE,
  MENU_ITEM_MAKE_LOWER_CASE,
  MENU_ITEM_CAPITALIZE,
  MENU_ITEM_CHANGE_BACK,
  MENU_ITEM_OPEN_MEDIA_IN_NEW_WINDOW,
  MENU_ITEM_COPY_MEDIA_LINK_TO_CLIPBOARD,
  MENU_ITEM_TOGGLE_MEDIA_CONTROLS,
  MENU_ITEM_TOGGLE_MEDIA_LOOP,
  MENU_ITEM_ENTER_VIDEO_FULLSCREEN,
  MENU_ITEM_MEDIA_PLAY_PAUSE,
  MENU_ITEM_MEDIA_MUTE,
  MENU_ITEM_DICTATION_ALTERNATIVE,
  MENU_ITEM_SELECT_ALL,
  MENU_ITEM_SELECT_WORD,
  MENU_ITEM_TEXT_SELECTION_MODE,
  MENU_ITEM_CLIPBOARD,
  MENU_ITEM_DRAG,
  MENU_ITEM_TRANSLATE
};

enum ContextMenuOptionType {
  MENU_ITEM_TYPE_ACTION,
  MENU_ITEM_TYPE_CHECKABLE_ACTION,
  MENU_ITEM_TYPE_SEPARATOR,
  MENU_ITEM_TYPE_SUBMENU
};

enum ContextMenuType {
  MENU_TYPE_LINK,
  MENU_TYPE_SELECTION
};

class ContextMenuItemEfl {
 public:
  ContextMenuItemEfl(ContextMenuOptionType item,
                     ContextMenuOption option,
                     const std::string& title,
                     const std::string& image_url = std::string(),
                     const std::string& link_url = std::string(),
                     const std::string& icon_path = std::string())
    : menu_type_(item)
    , menu_option_(option)
    , title_(title)
    , is_enabled_(true)
    , image_url_(image_url)
    , link_url_(link_url)
    , icon_path_(icon_path) {
  }

  ~ContextMenuItemEfl() { }

  const std::string& Title() const { return title_; }
  void SetTitle(const std::string& title) { title_ = title; }

  bool IsEnabled() const { return is_enabled_; }
  void SetEnabled(bool status) { is_enabled_ = status; }
  ContextMenuOption GetContextMenuOption() const { return menu_option_; }
  ContextMenuOptionType GetContextMenuOptionType() const { return menu_type_; }
  const std::string& LinkURL() const { return link_url_; }
  const std::string& ImageURL() const { return image_url_; }
  const std::string& IconPath() const { return icon_path_; }

 private:
  ContextMenuOptionType menu_type_;
  ContextMenuOption menu_option_;
  std::string title_;
  bool is_enabled_;
  std::string image_url_;
  std::string link_url_;
  std::string icon_path_;
};

class ContextMenuControllerEfl
    : public tizen_webview::ContextMenuController,
      public content::DownloadItem::Observer {
 public:
  static std::vector<ContextMenuItemEfl> _context_menu_listdata;

  static void ContextMenuCancelCallback(void* data, Evas_Object* obj, void* event_info);
  static void ContextMenuItemSelectedCallback(void* data, Evas_Object* obj, void* event_info);
  static void ContextMenuHWBackKey(void* data, Evas_Object* obj, void* event_info);

  ContextMenuControllerEfl(tizen_webview::WebView* wv, ContextMenuType type, WebContents& web_contents)
    : webview_(wv),
      popup_(NULL),
      widget_win_(NULL),
      top_widget_(NULL),
      menu_items_(NULL),
      type_(type),
      web_contents_(web_contents),
      weak_ptr_factory_(this) {
  }

  ~ContextMenuControllerEfl();

  bool PopulateAndShowContextMenu(const ContextMenuParams& params);
  void MenuItemSelected(ContextMenuItemEfl* menu_item);
  void HideContextMenu();
  gfx::Point GetContextMenuShowPos() const { return context_menu_show_pos_; };


 private:
  static void ContextMenuListTrackResize(void* data, Evas* e, Evas_Object* obj, void* event_info);
  static void ContextMenuListRealized(void* data, Evas_Object* obj, void* event_info);
  static char* ContextMenuGenlistTextSet(void* data, Evas_Object* obj, const char* part);
  static void ContextMenuPopupBoxResize(Evas_Object* top_widget, void* data);
  static void ContextMenuPopupResize(void* data, Evas* e, Evas_Object* obj, void* event_info);
  static void ContextMenuPopupRotationCallback(void* data, Evas_Object* obj, void* event_info);
  static void BlockClickedCallback(void*, Evas_Object*, void*);
  void CustomMenuItemSelected(ContextMenuItemEfl*);
  void GetProposedContextMenu();
  bool CreateContextMenu();
  bool ShowContextMenu();
  void AddItemToPropsedList(ContextMenuOptionType item,
                            ContextMenuOption option,
                            std::string title,
                            std::string image_url = std::string(),
                            std::string link_url = std::string(),
                            std::string icon_path = std::string());
  void HideSelectionHandle();
  void ShowSelectionHandleAndContextMenu();
  virtual void OnDownloadUpdated(content::DownloadItem* download) override;
  void OnDownloadStarted(content::DownloadItem* item, content::DownloadInterruptReason interrupt_reason);
  base::FilePath DownloadFile(const GURL url,
                              const base::FilePath outputDir,
                              const DownloadUrlParameters::OnStartedCallback& callback);
  bool TriggerDownloadCb(const GURL url);
  void OpenInNewTab(const GURL url);
  Evas_Object* GetWebViewEvasObject();
  void DeletePopup();

  static int _appended_item_size;
  static int _popup_item_height;
  static bool _context_menu_resized;
  tizen_webview::WebView* webview_;
  Evas_Object* popup_;
  Evas_Object* widget_win_;
  Evas_Object* top_widget_;
  Eina_List* menu_items_;
  ContextMenuType type_;
  ContextMenuParams params_;
  WebContents& web_contents_;
  base::WeakPtrFactory<ContextMenuControllerEfl> weak_ptr_factory_;
  gfx::Point context_menu_show_pos_;
  bool is_text_selection_;
  std::set<DownloadItem*> observed_download_items_;
};

} // namespace
#endif // context_menu_controller_h

