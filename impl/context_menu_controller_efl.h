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

#ifndef context_menu_controller_h
#define context_menu_controller_h

#include <Evas.h>
#include "content/public/common/context_menu_params.h"

namespace content {

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
  MENU_ITEM_SELECT_LINK,
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
                     const std::string& image_url,
                     const std::string& link_url,
                     const std::string& icon_path)
    : menu_type_(item)
    , menu_option_(option)
    , title_(title)
    , is_enabled_(true)
    , image_url_(image_url)
    , link_url_(link_url)
    , icon_path_(icon_path) {
  }

  ~ContextMenuItemEfl() { }

  const std::string& Title() { return title_; }
  void SetTitle(const std::string& title) { title_ = title; }

  bool IsEnabled() { return is_enabled_; }
  void SetEnabled(bool status) { is_enabled_ = status; }
  ContextMenuOption GetContextMenuOption() { return menu_option_; }
  ContextMenuOptionType GetContextMenuOptionType() { return menu_type_; }
  const std::string& LinkURL() { return link_url_; }
  const std::string& ImageURL() { return image_url_; }
  const std::string& IconPath() { return icon_path_; }

 private:
  ContextMenuOptionType menu_type_;
  ContextMenuOption menu_option_;
  std::string title_;
  bool is_enabled_;
  std::string image_url_;
  std::string link_url_;
  std::string icon_path_;
};

class ContextMenuControllerEfl {
 public:

  static void contextMenuCancelCallback(void* data, Evas_Object* obj, void* eventInfo);
  static void contextMenuItemSelectedCallback(void* data, Evas_Object* obj, void* eventInfo);

  ContextMenuControllerEfl(Evas_Object* evas_object, ContextMenuType type)
    : evas_object_(evas_object)
    , popup_(0)
    , menu_items_(0)
    , type_(type) {
  }

  ~ContextMenuControllerEfl();

  bool PopulateAndShowContextMenu(const ContextMenuParams &params);
  Evas_Object* GetPopUp() { return popup_; };
  void ResetPopUp() { popup_ = NULL; }
  void MenuItemSelected(ContextMenuItemEfl *menu_item);
  void HideContextMenu();

 private:
  void GetProposedContextMenu();
  bool CreateContextMenu();
  bool ShowContextMenu();
  void AddItemToPropsedList(ContextMenuOptionType item,
                            ContextMenuOption option,
                            std::string title,
                            std::string image_url,
                            std::string link_url,
                            std::string icon_path);

  Evas_Object* evas_object_;
  Evas_Object* popup_;
  Eina_List* menu_items_;
  ContextMenuType type_;
  ContextMenuParams params_;
};

} // namespace
#endif // context_menu_controller_h

