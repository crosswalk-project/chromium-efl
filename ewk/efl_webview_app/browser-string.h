/*
 *  browser
 *
 * Copyright (c) 2013 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Contact: Hyerim Bae <hyerim.bae@samsung.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef BROWSER_STRING_H
#define BROWSER_STRING_H

#include <app.h>

/* browser string define for translation */

/* System string */
/*************************************************************************************************************/
#define BR_STRING_OK            dgettext("sys_string", "IDS_COM_SK_OK")
#define BR_STRING_EDIT          dgettext("sys_string", "IDS_COM_SK_EDIT")
#define BR_STRING_YES           dgettext("sys_string", "IDS_COM_SK_YES")
#define BR_STRING_NO            dgettext("sys_string", "IDS_COM_SK_NO")
#define BR_STRING_BACK          dgettext("sys_string", "IDS_COM_BODY_BACK")
#define BR_STRING_CANCEL        dgettext("sys_string", "IDS_COM_SK_CANCEL")
#define BR_STRING_SAVE          dgettext("sys_string", "IDS_COM_SK_SAVE")
#define BR_STRING_DONE          dgettext("sys_string", "IDS_COM_SK_DONE")
#define BR_STRING_DELETE        dgettext("sys_string", "IDS_COM_SK_DELETE")
#define BR_STRING_DELETE_Q      dgettext("sys_string", "IDS_COM_POP_DELETE_Q")
#define BR_STRING_CLOSE         dgettext("sys_string", "IDS_COM_POP_CLOSE")
#define BR_STRING_ON            dgettext("sys_string", "IDS_COM_BODY_ON")
#define BR_STRING_OFF           dgettext("sys_string", "IDS_COM_BODY_OFF")
#define BR_STRING_WARNING       dgettext("sys_string", "IDS_COM_POP_WARNING")
#define BR_STRING_ERROR         dgettext("sys_string", "IDS_COM_POP_ERROR")
#define BR_STRING_PROCESSING    dgettext("sys_string", "IDS_COM_POP_PROCESSING")
#define BR_STRING_MESSAGES      dgettext("sys_string", "IDS_COM_BODY_MESSAGES")
#define BR_STRING_EMAIL         dgettext("sys_string", "IDS_COM_BODY_EMAIL")
#define BR_STRING_BLUETOOTH     dgettext("sys_string", "IDS_COM_BODY_BLUETOOTH")
#define BR_STRING_SMEMO         dgettext("sys_string", "IDS_COM_OPT_S_MEMO")
#define BR_STRING_BRIGHTNESS    dgettext("sys_string", "IDS_COM_OPT_BRIGHTNESS")
#define BR_STRING_AUTOMATIC     dgettext("sys_string", "IDS_COM_BODY_AUTOMATIC")
#define BR_STRING_INTERNET      dgettext("sys_string", "IDS_COM_BODY_INTERNET")
#define BR_STRING_CALL          dgettext("sys_string", "IDS_COM_BODY_CALL")
#define BR_STRING_NFC           dgettext("sys_string", "IDS_COM_BODY_NFC")
#define BR_STRING_DEFAULT       dgettext("sys_string", "IDS_COM_BODY_DEFAULT")
#define BR_STRING_LOW           dgettext("sys_string", "IDS_COM_BODY_LOW")
#define BR_STRING_WIFI          dgettext("sys_string", "IDS_COM_BODY_WI_FI")
#define BR_STRING_LOCATION      dgettext("sys_string", "IDS_COM_BODY_LOCATION")
#define BR_STRING_HOUR          dgettext("sys_string", "IDS_COM_BODY_1_HOUR")
#define BR_STRING_HOURS         dgettext("sys_string", "IDS_COM_POP_PD_HOURS")
#define BR_STRING_NO_NAME       dgettext("sys_string", "IDS_COM_BODY_NO_NAME")
#define BR_STRING_LOGIN         dgettext("sys_string", "IDS_COM_BUTTON_LOGIN")
#define BR_STRING_STREAMING_PLAYER dgettext("sys_string", "IDS_COM_BODY_STREAMING_PLAYER")
#define BR_STRING_TITLE_SELECT_AN_ACTION dgettext("sys_string", "IDS_COM_HEADER_SELECT_APPLICATIONS")
#define BR_STRING_DRAG_SCROLL	dgettext("sys_string","IDS_COM_BODY_DRAG_SCROLL_T_TTS")

/*************************************************************************************************************/


/* Browser common */
/*************************************************************************************************************/
#define BR_STRING_HISTORY               _("IDS_BR_TAB_HISTORY")
#define BR_STRING_ALLOW                 _("IDS_BR_OPT_ALLOW")
#define BR_STRING_ITEMS_SELECTED        _("IDS_BR_POP_PD_ITEMS_SELECTED")
#define BR_STRING_ONE_ITEM_SELECTED     _("IDS_BR_POP_1_ITEM_SELECTED")
#define BR_STRING_EMPTY                 _("IDS_BR_BODY_EMPTY")
#define BR_STRING_URL_GUIDE_TEXT        _("IDS_BR_BODY_SEARCH_OR_ENTER_URL")
#define BR_STRING_PRINT                 _("IDS_BR_BODY_PRINT")
#define BR_STRING_DATA_NETWORK          _("IDS_BR_OPT_DATA_NETWORK")
#define BR_STRING_CLOSE_WINDOW          _("IDS_BR_OPT_CLOSE_WINDOW")
#define BR_STRING_NO_TITLE              _("IDS_BR_BODY_NO_TITLE")
#define BR_STRING_LOADING_PLZ_WAIT      _("IDS_BR_POP_LOADING_PLEASE_WAIT_ING")
#define BR_STRING_LOAD_ERROR_TITLE      _("IDS_BR_HEADER_DATA_CONNECTIVITY_PROBLEM")
#define BR_STRING_LOAD_ERROR_MSG        _("IDS_BR_BODY_UNABLE_TO_DISPLAY_THE_WEBPAGE_NO_RESPONSE_FROM_THE_SERVER")
#define BR_STRING_NO_SEARCH_RESULT      _("IDS_BR_BODY_NO_RESULTS_FOUND")
#define BR_STRING_ADD                   _("IDS_BR_OPT_ADD")
#define BR_STRING_MOVE                  _("IDS_BR_OPT_MOVE")
#define BR_STRING_SHARE                 _("IDS_BR_OPT_SHARE")
#define BR_STRING_CLOSE_ALL             _("IDS_BR_OPT_CLOSE_ALL")
#define BR_STRING_NO_ITEMS              _("IDS_BR_BODY_NO_ITEMS")
#define BR_STRING_NEW_WINDOW            _("IDS_BR_OPT_NEW_WINDOW")
#define BR_STRING_CERTI_MESSAGE         _("IDS_BR_BODY_SECURITY_CERTIFICATE_PROBLEM_MSG")
#define BR_STRING_SECURE_CONNECTION     _("IDS_BR_POP_SECURE_CONNECTION")
#define BR_STRING_SECURITY_WARNING_MESSAGE _("IDS_BR_HEADER_SECURITY_WARNING")
#define BR_STRING_POP_CLOSE_ALL_OPENED_WINDOWS_AND_GO_TO_THE_HOMEPAGE _("IDS_BR_POP_CLOSE_ALL_OPENED_WINDOWS_AND_GO_TO_THE_HOMEPAGE")
#define BR_STRING_OPEN                  _("IDS_BR_OPT_OPEN")
#define BR_STRING_REMOVE		_("IDS_BR_OPT_REMOVE")
/*************************************************************************************************************/


/* Brightness */
/*************************************************************************************************************/
#define BR_STRING_HIGH                  _("IDS_BR_BODY_HIGH_MPRIORITY")
#define BR_STRING_MEDIUM                _("IDS_BR_BODY_MEDIUM")
#define BR_STRING_SETTINGS              _("IDS_BR_BODY_SETTINGS")
/*************************************************************************************************************/


/* Bookmark */
/*************************************************************************************************************/
#define BR_STRING_CREATE_FOLDER         _("IDS_BR_SK3_CREATE_FOLDER")
#define BR_STRING_CREATE_FOLDER_IN      _("IDS_BR_BODY_CREATE_FOLDER")
#define BR_STRING_FOLDER                _("IDS_BR_HEADER_FOLDER")
#define BR_STRING_ADD_TO_BOOKMARKS      _("IDS_BR_SK3_ADD_TO_BOOKMARKS")
#define BR_STRING_NO_BOOKMARKS          _("IDS_BR_BODY_NO_BOOKMARKS")
#define BR_STRING_TAG                   _("IDS_BR_BODY_ADDDETAILSVIEW_TAGS")
#define BR_STRING_UNTAGGED              _("IDS_BR_BODY_UNTAGGED")
#define BR_STRING_MOBILE                _("IDS_BR_BODY_MOBILE")
#define BR_STRING_POPUP_VIEW_BY         _("IDS_BR_OPT_VIEW_BY_ABB")
#define BR_STRING_ENTER_FOLDER_NAME     _("IDS_BR_BODY_ENTER_FOLDER_NAME")
#define BR_STRING_ENTER_BOOKMARK_NAME   _("IDS_BR_POP_ENTER_BOOKMARK_NAME") //hard coding in po file about japanese.
#define BR_STRING_ADDED_TO_BOOKMARKS    _("IDS_BR_POP_ADDED_TO_BOOKMARKS")
#define BR_STRING_REMOVED_TO_BOOKMARKS  _("IDS_BR_POP_REMOVED_FROM_BOOKMARKS")
#define BR_STRING_DOWNLOADING_ING       _("IDS_BR_POP_STARTING_DOWNLOAD_ING")
#define BR_STRING_SELECT_FOLDER         _("IDS_BR_BODY_SELECT_FOLDER")
#define BR_STRING_FOLDER_NAME           _("IDS_BR_BODY_FOLDER_NAME")
#define BR_STRING_ADD_BOOKMARK          _("IDS_BR_OPT_ADD_BOOKMARK")
#define BR_STRING_ADD_TAG               _("IDS_BR_HEADER_ADD_TAG")
#define BR_STRING_EDIT_TAGS             _("IDS_BR_POP_RENAME_TAG")
#define BR_STRING_ADD_NEW_FOLDER        _("IDS_BR_OPT_NEW_FOLDER")
#define BR_STRING_LIST_VIEW             _("IDS_BR_OPT_LIST_VIEW")
#define BR_STRING_THUMBNAIL_VIEW        _("IDS_BR_OPT_THUMBNAIL_VIEW")
#define BR_STRING_NO_TAG                _("IDS_BR_BODY_ADDDETAILS_HNO_TAGS")
#define BR_STRING_VIEW_BY_TAG           _("IDS_BR_OPT_SORT_BY_TAG")
#define BR_STRING_DELETE_BOOKMARK       _("IDS_BR_OPT_DELETE_BOOKMARK")
#define BR_STRING_RENAME_FOLDER         _("IDS_BR_OPT_RENAME_FOLDER")
#define BR_STRING_RENAME                _("IDS_BR_OPT_RENAME")
#define BR_STRING_ADD_TO_HOME_SCREEN    _("IDS_BR_OPT_ADD_TO_HOME_SCREEN_ABB")
#define BR_STRING_MOVE_Q                _("IDS_BR_BODY_MOVE_THE_SELECTED_BOOKMARKS_Q")
#define BR_STRING_GO_TO_HISTORY         _("IDS_BR_BUTTON2_GO_TO_HISTORY")
#define BR_STRING_DELETE_FOLDER         _("IDS_BR_OPT_DELETE_FOLDER")
#define BR_STRING_WARNING_OVER_TAG_LIMIT _("IDS_BR_BODY_MAXIMUM_NUMBER_OF_TAGS_HPD_REACHED")
#define BR_STRING_POP_ERR_MOVE_TO_SOURCE_FOLDER  _("IDS_BR_POP_THE_DESTINATION_FOLDER_IS_THE_SAME_AS_THE_SOURCE_FOLDER")
#define BR_STRING_POP_ERR_MOVE_TO_SUB_FOLDER    _("IDS_BR_POP_THE_DESTINATION_FOLDER_IS_A_SUBFOLDER_OF_THE_SOURCE_FOLDER")
#define BR_STRING_TARGET_FOLDER         _("IDS_BR_BODY_TARGET_FOLDER")
#define BR_STRING_TARGET_FOLDER_ABB         _("IDS_BR_BODY_TARGET_FOLDER_ABB")
#define BR_STRING_ADD_THIS_WEBSITE_TO_BOOKMARKS_TO_OPEN_IT_FASTER		_("IDS_BR_BODY_ADD_THIS_WEBSITE_TO_BOOKMARKS_TO_OPEN_IT_FASTER_ABB")
#define BR_STRING_EDIT_BOOKMARK         _("IDS_BR_HEADER_EDIT_BOOKMARK")
#define BR_STRING_1_POPUP_BLOCKED    _("IDS_BR_TPOP_1_POP_UP_BLOCKED")
#define BR_STRING_PD_POPUPS_BLOCKED    _("IDS_BR_BODY_PD_POP_UPS_BLOCKED_ABB")
/*************************************************************************************************************/


/* Hstory */
/*************************************************************************************************************/
#define BR_STRING_NO_HISTORY            _("IDS_BR_BODY_NO_HISTORIES")
#define BR_STRING_HISTORY_TODAY         _("IDS_BR_BODY_TODAY")
#define BR_STRING_HISTORY_YESTERDAY     _("IDS_BR_BODY_YESTERDAY")
#define BR_STRING_HISTORY_LAST_7_DAYS   _("IDS_BR_BODY_LAST_7_DAYS")
#define BR_STRING_HISTORY_LAST_MONTH    _("IDS_BR_BODY_LAST_MONTH")
#define BR_STRING_HISTORY_OLDER         _("IDS_BR_BODY_OLDER")
#define BR_STRING_CLEAR                 _("IDS_BR_SK_CLEAR")
/*************************************************************************************************************/


/* Html feature */
/*************************************************************************************************************/
#define BR_STRING_REQUEST_WEBPAGE_PREFIX _("IDS_BR_OPT_SAVEWEBPAGE")
#define BR_STRING_EXCEEDED_QUOTA_POPUP_DESC _("IDS_BR_POP_P1SS_HP2SS_IS_REQUESTING_PERMISSION_TO_STORE_DATA_ON_YOUR_DEVICE_FOR_OFFLINE_USE")
#define BR_STRING_APPCACHE_PERMISSION_POPUP_DESC _("IDS_BR_POP_P1SS_HP2SS_IS_ATTEMPTING_TO_STORE_A_LARGE_AMOUNT_OF_DATA_ON_YOUR_DEVICE_FOR_OFFLINE_USE")
#define BR_STRING_PROTOCOL_CONTENT_HANDLER_POPUP_DESC _("IDS_BR_POP_P1SS_HP2SS_IS_REQUESTING_PERMISSION_TO_CHANGE_HOW_YOUR_BROWSER_WORKS_ON_MSG")
#define BR_STRING_GEOLOCATION_PERMISSION_POPUP_DESC _("IDS_BR_POP_P1SS_HP2SS_IS_REQUESTING_PERMISSION_TO_ACCESS_YOUR_LOCATION")
#define BR_STRING_WEB_NOTI_PERMISSION_POPUP_DESC _("IDS_BR_POP_P1SS_HP2SS_IS_REQUESTING_PERMISSION_TO_SHOW_NOTIFICATIONS")
#define BR_STRING_USER_MEDIA_PERMISSION_POPUP_DESC _("IDS_BR_POP_P1SS_HP2SS_IS_REQUESTING_PERMISSION_TO_USE_YOUR_CAMERA")
/*************************************************************************************************************/


/* Shortcut */
/*************************************************************************************************************/
#define BR_STRING_SHORTCUT_CREATED      _("IDS_BR_POP_SHORTCUT_CREATED")
/*************************************************************************************************************/


/* More menu */
/*************************************************************************************************************/
#define BR_STRING_FIND_ON_PAGE              _("IDS_BR_OPT_FIND_ON_PAGE")
#define BR_STRING_BOOKMARKS                 _("IDS_BR_BODY_BOOKMARKS")
#define BR_STRING_BOOKMARK                  _("IDS_BR_OPT_BOOKMARK")
#define BR_STRING_ADD_BOOKMARKS             _("IDS_BR_OPT_ADDTOBOOKMARKS")
#define BR_STRING_BOOKMARK_ADDED            _("IDS_BR_POP_BOOKMARK_ADDED")
#define BR_STRING_BOOKMARK_REMOVED          _("IDS_BR_POP_BOOKMARK_REMOVED")
#define BR_STRING_SAVE_FOR_OFFLINE_READING  _("IDS_BR_BODY_SAVE_FOR_OFFLINE_READING_ABB")
#define BR_STRING_PRIVATE_ON                _("IDS_BR_BODY_ENABLE_PRIVATE_MODE")
#define BR_STRING_PRIVATE_OFF               _("IDS_BR_BODY_DISABLE_PRIVATE_MODE")
#define BR_STRING_SAVE_PAGE                 _("IDS_BR_OPT_SAVE_PAGE_ABB")
#define BR_STRING_DESKTOP_VIEW              _("IDS_BR_BODY_DESKTOP_VIEW")
#define BR_STRING_PRIVATE_BROWSER           _("IDS_BR_OPT_INCOGNITO_MODE_ABB")
#define BR_STRING_PRIVATE_BROWSER_DESC      _("IDS_BR_BODY_INTERNET_HISTORY_IS_NOT_STORED_ABB")
#define BR_STRING_SAVE_PAGE                 _("IDS_BR_OPT_SAVE_PAGE_ABB")
#define BR_STRING_SAVED_PAGE                _("IDS_BR_OPT_SAVEDPAGES")
#define BR_STRING_INSTALL_WEB_APP           _("IDS_BR_OPT_INSTALL_WEB_APP")
#define BR_STRING_MINIMIZE                  _("IDS_BR_OPT_MINIMISE")
#define BR_STRING_MOST_VISITED              _("IDS_BR_TAB_MOST_VISITED")
#define BR_STRING_MOBILE_VIEW               _("IDS_BR_BODY_MOBILE_VIEW")
#define BR_STRING_ADD_LIVEBOX               _("IDS_BR_OPT_ADD_LIVEBOX")
#define BR_STRING_ADD_TO_HOME               _("IDS_BR_OPT_ADD_TO_HOME")

#define BR_STRING_ADD_TO_WEB_CLIP           _("IDS_BR_OPT_ADD_TO_WEB_CLIP")
#define BR_STRING_ADD_SHORTCUT              _("IDS_BR_OPT_ADD_SHORTCUT")
#if defined(WEBCLIP)
#if 0
#define BR_STRING_WEB_CLIPPING              _("IDS_BR_BODY_WEB_CLIP")
#endif
#endif
/*************************************************************************************************************/


/* web clip */
/*************************************************************************************************************/
#define BR_STRING_NEVER                     _("IDS_BR_POP_NEVER")
/*************************************************************************************************************/

/* Quick Access */
/*************************************************************************************************************/
#define BR_STRING_NEVER _("IDS_BR_POP_NEVER")
#define BR_STRING_NO_SITE _("No site")
#define BR_STRING_ADD_TO_QUICK_ACCESS _("IDS_BR_OPT_ADD_TO_QUICK_ACCESS")
#define BR_STRING_PD_ITEMS_ADDED _("%d items added")
#define BR_STRING_1_ITEM_ADDED _("1 item added")
#define BR_STRING_NOTHING_ADDED _("Nothing added")
#define BR_STRING_QUICK_ACCESS _("IDS_BR_HEADER_QUICK_ACCESS_ABB")
#define BR_STRING_TRY_AGAIN_LATER _("Page is on loading. Please try again after load finished")
#define BR_STRING_ADD_BOOKMARK_TO_QUICK_ACCESS _("Add bookmark to quick access")
#define BR_STRING_ADDING_WEBPAGE_TO_QUICK_ACCESS _("IDS_BR_HEADER_ADDING_WEBPAGES_TO_QUICK_ACCESS")
#define BR_STRING_ADD_QUICK_ACCESS_ITEM _("Add quick access item")
#define BR_STRING_EDIT_QUICK_ACCESS_ITEM _("Edit quick access item")
#define BR_STRING_QUICK_ACCESS_SUCCESS_POPUP _("IDS_BR_TPOP_ADDED_TO_QUICK_ACCESS_LIST")
#define BR_STRING_QUICK_ACCESS_FAIL_POPUP _("Failed to save quick access item")
/*************************************************************************************************************/

/* Settings */
/*************************************************************************************************************/
#define BR_STRING_CACHE                     _("IDS_BR_OPT_CACHE")
#define BR_STRING_URL                       _("IDS_BR_BODY_URL")
#define BR_STRING_SELECT_ALL                _("IDS_BR_OPT_SELECT_ALL")
#define BR_STRING_GENERAL                   _("IDS_BR_BODY_GENARAL")
#define BR_STRING_HOMEPAGE                  _("IDS_BR_BODY_HOMEPAGE")
#define BR_STRING_HOMEPAGE_ABB              _("IDS_BR_BUTTON_HOMEPAGE_ABB")
#define BR_STRING_BLANK_PAGE                _("IDS_BR_BODY_BLANK_PAGE")
#define BR_STRING_CURRENT_PAGE              _("IDS_BR_BODY_CURRENT_PAGE")
#define BR_STRING_DEFAULT_PAGE              _("IDS_BR_BODY_DEFAULT_PAGE_ABB")
#define BR_STRING_USER_HOMEPAGE             _("IDS_BR_BODY_USER_HOMEPAGE")
#define BR_STRING_SET_HOME_URL              _("IDS_BR_ACHEADER2_SET_HOME_URL")
#define BR_STRING_SUPPORING_POPUP           _("IDS_BR_BODY_ADDITIONAL_WEBPAGE_INFO_ABB")
#define BR_STRING_AUTO_RESIZE_TEXT          _("IDS_BR_BODY_AUTO_RESIZE_TEXT_ABB")
#define BR_STRING_AUTO_RESIZE_TEXT_DESC     _("IDS_BR_SBODY_VIEW_PC_WEBSITES_EASILY_ON_YOUR_MOBILE_DEVICE_THIS_FEATURE_BOOSTS_THE_TEXT_SIZE_TO_MAKE_IT_EASIER_TO_READ_NOTE_C_THE_TEXT_LAYOUT_MAY_CHANGE")
#define BR_STRING_AUTO_FILL_FORMS           _("IDS_BR_MBODY_AUTO_FILL_FORMS")
#define BR_STRING_AUTO_FILL_FORMS_DESC      _("IDS_BR_BODY_SET_TEXT_FOR_WEB_FORM_AUTO_FILL")
#define BR_STRING_OPEN_PAGES_IN_OVERVIEW    _("IDS_BR_BODY_OPEN_PAGES_IN_OVERVIEW_ABB")
#define BR_STRING_OPEN_PAGES_IN_OVERVIEW_DESC _("IDS_BR_BODY_SHOW_OVERVIEW_OF_NEWLY_OPENED_PAGES")
#define BR_STRING_CONTENTS                  _("IDS_BR_HEADER_CONTENTS")
#define BR_STRING_DEFAULT_VIEW_LEVEL        _("IDS_BR_BODY_DEFAULT_VIEW")
#define BR_STRING_FIT_TO_WIDTH              _("IDS_BR_BODY_FIT_TO_WIDTH")
#define BR_STRING_READABLE                  _("IDS_BR_BODY_READABLE")
#define BR_STRING_RUN_JAVASCRIPT            _("IDS_BR_BODY_RUN_JAVASCRIPT")
#define BR_STRING_RUN_JAVASCRIPT_DESC       _("IDS_BR_BODY_ALLOW_SITES_TO_RUN_JAVASCRIPT")
#define BR_STRING_SHOW_IMAGES               _("IDS_BR_BODY_SHOW_IMAGES")
#define BR_STRING_SHOW_IMAGES_DESC          _("IDS_BR_BODY_DISPLAY_IMAGES_ON_WEB_PAGES")
#define BR_STRING_ALWAYS_ON                 _("IDS_BR_BODY_ALWAYS_ON")
#define BR_STRING_BLOCK_POPUP_DESC          _("IDS_BR_BODY_BLOCK_POP_UPS_ON_WEB_PAGES")
#define BR_STRING_PRIVACY_AND_SECURTY       _("IDS_BR_BODY_PRIVACY_AND_SECURITY_ABB")
#define BR_STRING_ALWAYS_ASK                _("IDS_BR_BODY_ALWAYS_ASK")
#define BR_STRING_DELETE_BROWSING_DATA      _("IDS_BR_BODY_DELETE_BROWSING_DATA")
#define BR_STRING_FONT_SIZE                 _("IDS_BR_BODY_FONT_SIZE")
#define BR_STRING_FONT_SIZE_NORMAL          _("IDS_BR_BODY_NORMAL")
#define BR_STRING_FONT_SIZE_SMALL           _("IDS_BR_BODY_SMALL")
#define BR_STRING_FONT_SIZE_HUGE            _("IDS_BR_OPT_HUGE")
#define BR_STRING_FONT_SIZE_LARGE           _("IDS_BR_BODY_LARGE")
#define BR_STRING_FONT_SIZE_GIANT           _("IDS_BR_OPT_GIANT_M_TEXTSIZE")
#define BR_STRING_CLEAR_CACHE               _("IDS_BR_BODY_CLEAR_CACHE")
#define BR_STRING_CLEAR_ALL_CACHE_DATA_Q    _("IDS_BR_POP_DELETE_LOCALLY_CACHED_CONTENT_AND_DATABASES_Q")
#define BR_STRING_CLEAR_HISTORY             _("IDS_BR_BODY_CLEAR_HISTORY")
#define BR_STRING_CLEAR_HISTORY_Q           _("IDS_BR_POP_CLEAR_HISTORY_Q")
#define BR_STRING_CLEAR_ALL_HISTORY_DATA_Q  _("IDS_BR_BODY_DELETE_BROWSER_HISTORY_Q")
#define BR_STRING_SHOW_SECURITY_WARNINGS    _("IDS_BR_BODY_SHOW_SECURITY_WARNINGS")
#define BR_STRING_COOKIES                   _("IDS_BR_BODY_COOKIES")
#define BR_STRING_ACCEPT_COOKIES            _("IDS_BR_BODY_ACCEPT_COOKIES")
#define BR_STRING_ACCEPT_COOKIES_DESC       _("IDS_BR_BODY_ALLOWS_SITES_TO_SAVE_AND_READ_COOKIE_DATA")
#define BR_STRING_CLEAR_ALL_COOKIE_DATA     _("IDS_BR_BODY_CLEAR_ALL_COOKIE_DATA")
#define BR_STRING_CLEAR_ALL_COOKIE_DATA_Q   _("IDS_BR_POP_EMPTY_COOKIE_POPUP_BNO")
#define BR_STRING_PASSWORD                  _("IDS_BR_BODY_PASSWORD")
#define BR_STRING_ENABLE_LOCATION           _("IDS_BR_BODY_ENABLE_LOCATION")
#define BR_STRING_ENABLE_LOCATION_DESC      _("IDS_BR_BODY_ALLOW_SITES_TO_REQUEST_ACCESS_TO_YOUR_LOCATION")
#define BR_STRING_CLEAR_LOCATION_ACCESS     _("IDS_BR_BODY_CLEAR_LOCATION_ACCESS")
#define BR_STRING_CLEAR_ALL_LOCATION_DATA_Q _("IDS_BR_POP_DELETE_WEBSITE_LOCATION_ACCESS_INFORMATION_Q")
#define BR_STRING_FORMDATA                  _("IDS_BR_HEADER_FORM_DATA")
#define BR_STRING_CLEAR_STORED_DATA         _("IDS_BR_BODY_CLEAR_STORED_DATA")
#define BR_STRING_REMEMBER_FORMDATA         _("IDS_BR_BODY_REMEMBER_FORM_DATA")
#define BR_STRING_REMEMBER_FORMDATA_DESC    _("IDS_BR_BODY_REMEMBER_DATA_I_TYPE_IN_FORMS_FOR_LATER_USE")
#define BR_STRING_CLEAR_FORMDATA            _("IDS_BR_BODY_CLEAR_FORM_DATA")
#define BR_STRING_CLEAR_ALL_FORMDATA_Q      _("IDS_BR_POP_DELETE_ALL_SAVED_FORM_DATA_Q")
#define BR_STRING_REMEMBER_PASSWORDS        _("IDS_BR_BODY_REMEMBER_PASSWORDS")
#define BR_STRING_REMEMBER_PASSWORDS_DESC   _("IDS_BR_BODY_SAVE_USER_NAMES_AND_PASSWORDS_FOR_WEBSITES")
#define BR_STRING_CLEAR_PASSWORDS           _("IDS_BR_BODY_CLEAR_PASSWORDS")
#define BR_STRING_SEARCH                    _("IDS_BR_BODY_SEARCH")
#define BR_STRING_DRAG_AND_DROP             _("IDS_BR_OPT_DRAG_AND_DROP")
#define BR_STRING_WEBSITE_SETTINGS          _("IDS_BR_BODY_WEBSITE_SETTINGS")
#define BR_STRING_WEBSITE_SETTINGS_DESC     _("IDS_BR_BODY_ADVANCED_SETTINGS_FOR_INDIVIDUAL_WEBSITES")
#define BR_STRING_RESET_TO_DEFAULT          _("IDS_BR_BODY_RESET_TO_DEFAULT")
#define BR_STRING_RESTORE_DEFAULT_SETTING_Q _("IDS_BR_BODY_RESTORE_DEFAULT_SETTINGS_Q")
#define BR_STRING_RESET_TO_DEFAULT_Q        _("IDS_BR_POP_ALL_SETTINGS_TO_DEFAULT_CONTINUE_Q")
#define BR_STRING_ABOUT_BROWSER             _("IDS_BR_BODY_ABOUT_BROWSER")
#define BR_STRING_SAVED_ID_PASSWORD         _("IDS_BR_BODY_AUTO_SAVE_ID_PASSWORD")
#define BR_STRING_SEARCH_ENGINE             _("IDS_BR_BODY_SEARCH_ENGINE")
#define BR_STRING_USER_AGENT                _("IDS_BR_HEADER_USER_AGENT")
#define BR_STRING_CUSTOM_USER_AGENT         _("IDS_BR_BODY_CUSTOM_USER_AGENT")
#define BR_STRING_DELETE_ALL                _("IDS_BR_BODY_DELETE_ALL")
#define BR_STRING_SAVED_ID_AND_PASSWORDS    _("IDS_BR_BODY_SAVED_ID_PASSWORD")
#define BR_STRING_MOST_VISITED_SITES        _("IDS_BR_BODY_MOST_VISITED_SITES")
#define BR_STRING_MSG_MDM_POLICY            _("IDS_BR_POP_SECURITY_POLICY_RESTRICTS_USE_OF_INTERNET_BROWSER")
#define BR_STRING_CLEAR_DATA                _("IDS_ST_BODY_CLEAR_DATA")
#define BR_STRING_CLEAR_PERSONALISED_DATA   _("IDS_BR_HEADER_CLEAR_PERSONALISED_DATA")
#define BR_STRING_HTML5_EXTENSION           _("IDS_BR_HEADER_HTML5_EXTENSION")
#define BR_STRING_CLEAR_ALL_SAVED_PASSWORDS_Q           _("IDS_BR_POP_DELETE_ALL_SAVED_PASSWORDS_Q")
#define BR_STRING_ENCODING_TYPE                         _("IDS_BR_OPT_TEXT_ENCODING")
#define BR_STRING_ENCODING_TYPE_LATIN_1_ISO_8859_1      _("IDS_BR_OPT_LATIN_1_HISO_8859_1")
#define BR_STRING_ENCODING_TYPE_UNICODE_UTF_8           _("IDS_BR_OPT_UNICODE_HUTF_8")
#define BR_STRING_ENCODING_TYPE_CHINESE_BIG5            _("IDS_BR_OPT_CHINESE_HBIG5")
#define BR_STRING_ENCODING_TYPE_CHINESE_GBK             _("IDS_BR_OPT_CHINESE_HGBK")
#define BR_STRING_ENCODING_TYPE_JAPANESE_SHIFT_JIS      _("IDS_BR_OPT_JAPANESE_HSHIFT_JIS")
#define BR_STRING_ENCODING_TYPE_JAPANESE_EUC_JP         _("IDS_BR_OPT_JAPANESE_HEUC_JP")
#define BR_STRING_ENCODING_TYPE_JAPANESE_ISO_2022_JP    _("IDS_BR_OPT_JAPANESE_HISO_2022_JP")
#define BR_STRING_ENCODING_TYPE_KOREAN_EUC_KR           _("IDS_BR_OPT_KOREAN_HEUC_KR")
#define BR_STRING_ENCODING_TYPE_AUTOMATIC               _("IDS_BR_BODY_AUTOMATIC")
#define BR_STRING_DELETE_ALL_WEBSITE_DATA_AND_LOCATION_PERMISSIONS _("IDS_BR_POP_DELETE_ALL_WEBSITE_DATA_AND_LOCATION_PERMISSIONS_Q")
#define BR_STRING_LOCATION_INFO		_("IDS_BR_BODY_LOCATION_M_INFORMATION")
#define BR_STRING_ENTER_ZIPCODE	_("IDS_BR_BODY_ENTER_POSTCODE")
#define BR_STRING_ALWAYS_SHOW _("IDS_BR_BUTTON_ALWAYS_SHOW_ABB")
/*************************************************************************************************************/


/* Popup, inform and warnings */
/*************************************************************************************************************/
#define BR_STRING_DELETED                       _("IDS_BR_POP_DELETED")
#define BR_STRING_ENTER_URL                     _("IDS_BR_POP_ENTER_URL")
#define BR_STRING_ALREADY_EXISTS                _("IDS_BR_POP_ALREADY_EXISTS")
#define BR_STRING_FAILED                        _("IDS_BR_POP_FAIL")
#define BR_STRING_SAVED                         _("IDS_BR_POP_SAVED")
#define BR_STRING_MOVED                         _("IDS_BR_POP_MOVED")
#define BR_STRING_ENTER_TITLE                   _("IDS_BR_POP_ENTER_TITLE_ABB")
#define BR_STRING_NETWORK_ERROR                 _("IDS_BR_POP_NETWORK_ERROR")
#define BR_STRING_NOT_FOUND_URL                 _("IDS_BR_POP_URL_NOT_FOUND")
#define BR_STRING_REFUSE                        _("IDS_BR_POP_REFUSE")
#define BR_STRING_AUTH_REQUIRED                 _("IDS_BR_BODY_DESTINATIONS_AUTHENTICATION_REQUIRED")
#define BR_STRING_USER_NAME                     _("IDS_BR_BODY_AUTHUSERNAME")
//#define BR_STRING_PASSWORD                    _("IDS_BR_BODY_AUTHPASSWORD")
#define BR_STRING_NEVER_SHOW_AGAIN              _("IDS_BR_POP_DO_NOT_SHOW_AGAIN")
#define BR_STRING_ENABLE_NETWORK_Q              _("IDS_BR_POP_MOBILE_DATA_DISABLED_CONNECT_USING_WI_FI_NETWORK_OR_ENABLE_MOBILE_DATA")//
#define BR_STRING_WARNING_OVER_BOOKMARK_LIMIT   _("IDS_BR_POP_UNABLE_TO_ADD_BOOKMARK_MAXIMUM_NUMBER_OF_BOOKMARKS_REACHED")
#define BR_STRING_MAX_WINDOW_WARNING	        _("IDS_BR_BODY_COULD_NOT_OPEN_NEW_WINDOW_BECAUSE_YOU_HAVE_ALREADY_OPENED_MAXIMUM_NUMBER")
#define BR_STRING_INSERT_SIM_CARD               _("IDS_BR_POP_PLEASE_INSERT_SIM_CARD")
#define BR_STRING_SET_CURRENT_PAGE_AS_HOMEPAGE  _("IDS_BR_BODY_SET_CURRENT_PAGE_AS_HOMEPAGE")
#define BR_STRING_CREATED                       _("IDS_BR_BODY_CREATED")
#define BR_STRING_AUTO_REFRESH                  _("IDS_BR_HEADER_AUTO_REFRESH")
#define BR_STRING_INVALID_URL                   _("IDS_BR_POP_INVALID_URL")
#define BR_STRING_SELECT_AVAILABLE_APP          _("IDS_BR_OPT_SELECT")
#define BR_STRING_DOWNLOAD_COMPLETE             _("IDS_BR_POP_DOWNLOAD_COMPLETED")
#define BR_STRING_NO_NETWORK_CONNECTION         _("IDS_BR_HEADER_NO_NETWORK_CONNECTION")
#define BR_STRING_FLIGHT_MODE_WARNING           _("IDS_BR_POP_FLIGHT_MODE_ENABLED_USING_PACKET_DATA_WILL_DISABLE_FLIGHT_MODE")
#define BR_STRING_HTTP_URL_CAN_BE_DOWNLOADED    _("IDS_BR_POP_ONLY_HTTP_OR_HTTPS_URLS_CAN_BE_DOWNLOADED")
#define BR_STRING_HTML5_FEATURE_NOT_SUPPORT_POPUP _("IDS_BR_POP_SOME_OLD_HTML_WEBSITES_MAY_NOT_SUPPORT_THIS_FEATURE")
#define BR_STRING_TAP_ONCE_MORE_TO_CLOSE_INTERNET _("IDS_BR_BODY_TAP_ONCE_MORE_TO_CLOSE_INTERNET")
#define BR_STRING_MSG_DELETE_WEBSITE_LOCATION_ACCESS_INFORMATION_Q _("IDS_BR_POP_DELETE_WEBSITE_LOCATION_ACCESS_INFORMATION_Q")
#define BR_STRING_AFTER_YOU_SAVE_SOME_PAGES_THE_SAVED_PAGES_WILL_BE_SHOWN _("IDS_BR_BODY_AFTER_YOU_SAVE_SOME_PAGES_THE_SAVED_PAGES_WILL_BE_SHOWN")
#define BR_STRING_SET_AS_HOMEPAGE _("IDS_BR_POP_SET_AS_HOMEPAGE")
/*************************************************************************************************************/


/* web notification */
/*************************************************************************************************************/
#define BR_STRING_WEB_NOTIFICATION              _("IDS_BR_HEADER_WEB_NOTIFICATION")
#define BR_STRING_WEB_NOTI_ALLOW_Q              _("IDS_BR_BODY_ALLOW_PS_TO_SHOW_NOTIFICATIONS")
#define BR_STRING_WEB_NOTI_PERMISSION_SETTING   _("IDS_BR_BODY_CLEAR_WEB_NOTIFICATION_PERMISSION_SETTINGS")
#define BR_STRING_WEB_NOTI_PERMISSION_Q         _("IDS_BR_BODY_CLEAR_WEB_NOTIFICATION_PERMISSIONS_Q")
/*************************************************************************************************************/


/* Auto fill form */
/*************************************************************************************************************/
#define BR_STRING_PROFILE                       _("IDS_BR_BODY_PROFILE")
#define BR_STRING_PROFILES                      _("IDS_BR_HEADER_PROFILES")
#define BR_STRING_AUTO_FILL_DATA_FULL_NAME      _("IDS_BR_BODY_FULL_NAME_ABB")
#define BR_STRING_AUTO_FILL_DATA_COMPANY_NAME   _("IDS_BR_BODY_COMPANY_NAME_ABB")
#define BR_STRING_AUTO_FILL_DATA_ADDRESS_LINE_1 _("IDS_BR_BODY_ADDRESS_LINE_1_ABB")
#define BR_STRING_AUTO_FILL_DATA_ADDRESS_LINE_2 _("IDS_BR_BODY_ADDRESS_LINE_2_ABB")
#define BR_STRING_AUTO_FILL_DATA_CITY_TOWN      _("IDS_BR_BODY_CITY_TOWN_ABB")
#define BR_STRING_AUTO_FILL_DATA_COUNTRY_REGION _("IDS_BR_BODY_COUNTY_ABB")
#define BR_STRING_AUTO_FILL_DATA_COUNTRY        _("IDS_BR_BODY_COUNTRY_REGION_ABB2")
#define BR_STRING_AUTO_FILL_DATA_PHONE          _("IDS_BR_BODY_PHONE")
#define BR_STRING_AUTO_FILL_DATA_EMAIL          _("IDS_BR_BODY_EMAIL_ADDRESS")
#define BR_STRING_PRIMARY_ADDRESS_GUIDE_TEXT    _("IDS_BR_BODY_STREET_ADDRESS_PO_BOX_C_O_ETC_ABB")
#define BR_STRING_SECONDARY_ADDRESS_GUIDE_TEXT  _("IDS_BR_BODY_FLAT_SUITE_UNIT_BUILDING_FLOOR_ETC_ABB")
#define BR_STRING_ENTER_NAME                    _("IDS_BR_BODY_ENTER_NAME")
#define BR_STRING_ENTER_YOUR_NAME               _("IDS_BR_BODY_ENTER_YOUR_NAME_ABB")
#define BR_STRING_ENTER_COMPANY_NAME            _("IDS_BR_BODY_ENTER_COMPANY_NAME_ABB")
#define BR_STRING_ENTER_TOWN_CITY_COUNTY        _("IDS_BR_BODY_ENTER_TOWN_CITY_COUNTY_ABB")
#define BR_STRING_ENTER_POSTCODE                _("IDS_BR_BODY_ENTER_POSTCODE_ABB")
#define BR_STRING_ENTER_COUNTRY_REGION          _("IDS_BR_BODY_ENTER_COUNTRY_REGION_ABB")
#define BR_STRING_ENTER_PHONE_NUMBER            _("Enter your phone number")
#define BR_STRING_ENTER_EMAIL_ADDRESS           _("Enter your Email address")
#define BR_STRING_TOWN_AUTO_FILL_CITY_COUNTY            _("IDS_BR_BODY_TOWN_CITY_COUNTY_ABB")
/*************************************************************************************************************/


/* Hard coded text */
/*************************************************************************************************************/

#define BR_STRING_SHORTCUT              _("Shortcut") //not in use.
////////tag////////
#define BR_STRING_TAP_TO_ADD_TAG        _("Tap to add tag") //not in use.
#define BR_STRING_TAP_TO_INPUT_TAG      _("Tap to input tag") //not in use.
#define BR_STRING_REMOVE_TAG        _("Remove tag") //not in use.
#define BR_STRING_SPECIAL_CHARACTOR_WARNING        	_("Special charactor can't be included")//not in ux guide

///////html5////////
#define BR_STRING_FAILED_TO_CREATE_AS_WEBAPP _("Failed to create as webapp")
#define BR_STRING_USERMEDIA                     _("This application wants to use your media.")//registering2
#define BR_STRING_FAILED_TO_SEND_VIA_BT         _("Failed to share via Bluetooth")//not use. It will be remove.
#define BR_STRING_NFC_CONNECT_FAILED    _("Failed to get NFC connection")//not use. It will be remove.
#define BR_STRING_FAILED_TO_USE_SBEAM   _("Failed to use S beam. try later")//not use. It will be remove.
#define BR_STRING_WARNING_VIDEO_PLAYER _("Can not launch video-player while video-call is running.")//not use. It will be remove.
#define BR_STRING_STILL_WEBAPP_MAKING               _("Still installing previous webapp")//not use. It will be remove.
#define BR_STRING_UNABLE_TO_SHARE_FILE_VIA_MESSAGE _("Unable to share file via message")///not use. It will be remove.
#define BR_STRING_KEEP_IT              _("IDS_BR_OPT_SAVE_TO_KEEPIT_ABB")
#define BR_STRING_ALLOW_OFFLINE_CACHE_Q _("Allow %s to use offline application cache?")
#define BR_STRING_ALLOW_WEB_DATABASE_SIZE_SET_Q _("Allow %s to open %s use up to %d of web database?")
#define BR_STRING_INDEXED_DB_SIZE_SET_TO_2GB_Q _("%s used %d of storage. Allow %s to use up to 2GB of indexed db?")
#define BR_STRING_FILE_SYSTEM_SIZE_SET_TO_2GB_Q _("%s used %d of storage. Allow %s to use up to 2GB of filesystem?")

/* New string */
/*************************************************************************************************************/
// If you need a string, please enter text below.
/*************************************************************************************************************/
// csr-framework
#if defined(CSRFW)
#define BR_STRING_TCS_GO_BACK _("IDS_BR_BUTTON_BACK_ABB")
#define BR_STRING_TCS_DESCRIPTION_LEVEL0 _("IDS_BR_POP_P1SS_HAS_BEEN_DETECTED_DETECTED_IN_C_P2SS_OPEN_WEBPAGE_ANYWAY_Q")
#define BR_STRING_TCS_DESCRIPTION_LEVEL1 _("IDS_BR_POP_P1SS_HAS_BEEN_DETECTED_DETECTED_IN_C_P2SS_SOME_CONTENT_HAS_BEEN_BLOCKED_TO_PROTECT_YOUR_DEVICE")
#endif
#define BR_STRING_REORDER                     _("IDS_BR_OPT_REORDER_ABB")
#define BR_STRING_FAILED_TO_GET_WEB_NOTI_ICON _("Failed to get web notification icon")
#define BR_STRING_READER_AVAILABLE _("Reader available")
#define BR_STRING_DELETE_FOLDER                     _("IDS_BR_OPT_DELETE_FOLDER")
#define BR_STRING_MSG_WEBCLIP_BLOCKED _("IDS_BR_BODY_UNABLE_TO_ADD_TO_HOME_SCREEN_THIS_WEBSITE_DOES_NOT_ALLOW_IT_TO_BE_PREVIEWED_ABB")
#define BR_STRING_LOW_MEMORY_WARNING _("Low memory, Can't launch browser. Kill other applications")
//#define BR_STRING_WINDOW_MANAGER _("IDS_BR_BODY_WINDOW_MANAGER")
#define BR_STRING_WINDOWS	_("IDS_BR_OPT_WINDOWS")
#define BR_STRING_DEVELOPER_OPTIONS _("IDS_BR_BODY_DEVELOPER_MODE")
#define BR_STRING_SYNC_OPEN_PAGES _("IDS_BR_BODY_SYNC_OPEN_PAGES")
#define BR_STRING_CTXMENU_SHARE_IMAGE                   _("Share image")
#define BR_STRING_CTXMENU_PASTE_AND_GO                  _("Paste & Go")
#define BR_STRING_WEBPROCESS_CRASH              _("WebProcess is crashed")
#define BR_STRING_AUTO_FILL_DATA_POST_CODE _("IDS_BR_BODY_POSTCODE_ABB")
#define BR_STRING_FORCE_ZOOM _("IDS_BR_BODY_FORCE_ZOOM_ABB")
#define BR_STRING_ALWAYS_ON _("IDS_BR_BODY_ALWAYS_ON")
#define BR_STRING_ON_DEMAND _("IDS_BR_BODY_ON_DEMAND")
#define BR_STRING_ACCOUNT_VERIFICATION_REQUIRED _("Your account need to be verified to sync bookmarks or tabs.") // FIXME: Temporary string.
#define BR_STRING_CLOSE_APPLICATION_Q _("IDS_BR_POP_CLOSE_APPLICATION_Q")

/*************************************************************************************************************/


/* New string for new settings */
/*************************************************************************************************************/
#define BR_STRING_SETTINGS_BASICS _("IDS_BR_BODY_BASIC")
#define BR_STRING_GENERAL_SETTINGS	_("IDS_BR_BODY_GENERAL_M_SETTINGS")
#define BR_STRING_SETTINGS_ACCOUNT _("IDS_BR_POP_ACCOUNT")
#define BR_STRING_SETTINGS_SET_HOMEPAGE _("IDS_BR_MBODY_SET_HOMEPAGE")
#define BR_STRING_SETTINGS_SYNC_SAMSUNG_ACCOUNT_NAME _("Samsung")
#define BR_STRING_SETTINGS_SYNC_INTERNET_DATA _("IDS_BR_BODY_SYNC_INTERNET_DATA")
#define BR_STRING_SETTINGS_SYNC_OPEN_PAGES _("IDS_BR_BODY_SYNC_OPEN_PAGES")
#define BR_STRING_SETTINGS_SYNC_BOOKMARKS _("IDS_BR_BODY_SYNC_BOOKMARKS_ABB")
#define BR_STRING_SETTINGS_AUTO_FILL_FORMS _("IDS_BR_MBODY_AUTO_FILL_FORMS")
#define BR_STRING_SETTINGS_AUTO_FILL_FORMS_DESC _("IDS_BR_BODY_SET_TEXT_FOR_WEB_FORM_AUTO_FILL")
#define BR_STRING_SETTINGS_ADVANCED _("IDS_BR_BODY_ADVANCED")
#define BR_STRING_SETTINGS_PRIVACY _("IDS_BR_BODY_PRIVACY")
#define BR_STRING_SETTINGS_SCREEN_AND_TEXT _("IDS_BR_BODY_SCREEN_AND_TEXT")
#define BR_STRING_SETTINGS_CONTENT_SETTINGS _("IDS_BR_BODY_CONTENT_SETTINGS")
#define BR_STRING_SETTINGS_BANDWIDTH_MANAGEMENT _("IDS_BR_HEADER_BANDWIDTH_MANAGEMENT_ABB")
#define BR_STRING_SETTINGS_DEVELOPER_MODE _("IDS_BR_BODY_DEVELOPER_MODE")

#define BR_STRING_SETTINGS_DEFAULT_PAGE _("IDS_BR_BODY_DEFAULT_PAGE_ABB")
#define BR_STRING_SETTINGS_CURRENT_PAGE _("IDS_BR_BODY_CURRENT_PAGE")
#define BR_STRING_SETTINGS_BLANK_PAGE _("IDS_BR_BODY_BLANK_PAGE")
#define BR_STRING_SETTINGS_MOST_VISITED_SITES _("IDS_BR_BODY_MOST_VISITED_SITES")
#define BR_STRING_SETTINGS_QUICK_ACCESS BR_STRING_QUICK_ACCESS
#define BR_STRING_SETTINGS_SET_HOME_URL _("IDS_BR_MBODY_SET_HOME_URL")
#define BR_STRING_SETTINGS_OTHER _("IDS_BR_BODY_OTHER_ABB")

#define BR_STRING_SETTINGS_ADD_PROFILE _("IDS_BR_MBODY_ADD_PROFILE")
#define BR_STRING_SETTINGS_EDIT_PROFILE _("IDS_BR_HEADER_EDIT_PROFILE")

#define BR_STRING_SETTINGS_SEARCH_AND_URL_SUGGESTIONS _("IDS_BR_MBODY_URL_SUGGESTIONS")
#define BR_STRING_SETTINGS_SEARCH_AND_URL_SUGGESTIONS_DESC _("IDS_BR_BODY_USE_A_PREDICTION_SERVICE_TO_DISPLAY_RELATED_QUERIES_AND_POPULAR_WEBSITES_IN_THE_ADDRESS_BAR_AS_YOU_TYPE")
#define BR_STRING_SETTINGS_PRELOAD_AVAILABLE_LINKS_BEFORE _("IDS_BR_MBODY_PRELOAD_LINKED_WEBPAGES")
#define BR_STRING_SETTINGS_PRELOAD_AVAILABLE_LINKS_BEFORE_DESC _("IDS_BR_BODY_IMPROVE_PERFORMANCE_WHEN_LOADING_PAGES")
#define BR_STRING_SETTINGS_SECURITY_WARNINGS _("IDS_BR_BODY_SHOW_SECURITY_WARNINGS")
#define BR_STRING_SETTINGS_SECURITY_WARNINGS_DESC _("IDS_BR_BODY_DISPLAY_SECURITY_WARNING_POP_UPS")
#define BR_STRING_SETTINGS_REMEMBER_FORM_DATA _("IDS_BR_BODY_REMEMBER_FORM_DATA")
#define BR_STRING_SETTINGS_REMEMBER_FORM_DATA_DESC _("IDS_BR_BODY_REMEMBER_DATA_I_TYPE_IN_FORMS_FOR_LATER_USE")
#define BR_STRING_SETTINGS_REMEMBER_PASSWORDS _("IDS_BR_BODY_REMEMBER_PASSWORDS")
#define BR_STRING_SETTINGS_REMEMBER_PASSWORDS_DESC _("IDS_BR_BODY_SAVE_USER_NAMES_AND_PASSWORDS_FOR_WEBSITES")
#define BR_STRING_SETTINGS_INCOGNITO_MODE _("IDS_BR_OPT_INCOGNITO_MODE_ABB")
#define BR_STRING_SETTINGS_INCOGNITO_MODE_DESC _("IDS_BR_BODY_INTERNET_HISTORY_IS_NOT_STORED_ABB")
#define BR_STRING_SETTINGS_DELETE_PERSONAL_DATA _("IDS_BR_BODY_DELETE_PERSONAL_DATA")
#define BR_STRING_SETTINGS_DELETE_PERSONAL_DATA_DESC _("IDS_BR_HEADER_CLEAR_PERSONALISED_DATA")

#define BR_STRING_SETTINGS_PREVIEW _("Preview")
#define BR_STRING_SETTINGS_PREVIEW_DESC _("Changed settings will be applied on here")
#define BR_STRING_SETTINGS_TEXT_SCAILING BR_STRING_FONT_SIZE//_("Text scailing")
#define BR_STRING_SETTINGS_FORCE_ZOOM _("IDS_BR_BODY_FORCE_ZOOM_ABB")
#define BR_STRING_SETTINGS_FORCE_ZOOM_DESC _("IDS_BR_BODY_OVERRIDE_WEBSITES_REQUEST_TO_CONTROL_ZOOM")
#define BR_STRING_SETTINGS_TEXT_ENCODING _("IDS_BR_OPT_TEXT_ENCODING")
#define BR_STRING_SETTINGS_FULL_SCREEN _("IDS_BR_BODY_FULL_SCREEN")
#define BR_STRING_SETTINGS_FULL_SCREEN_DESC _("IDS_BR_BODY_USE_FULL_SCREEN_MODE_TO_HIDE_THE_STATUS_BAR")
#define BR_STRING_SETTINGS_TEXT_BOOSTER _("IDS_BR_MBODY_TEXT_BOOSTER")
#define BR_STRING_SETTINGS_TEXT_BOOSTER_DESC _("IDS_BR_SBODY_VIEW_PC_WEBSITES_EASILY_ON_YOUR_MOBILE_DEVICE_THIS_FEATURE_BOOSTS_THE_TEXT_SIZE_TO_MAKE_IT_EASIER_TO_READ_NOTE_C_THE_TEXT_LAYOUT_MAY_CHANGE")

#define BR_STRING_SETTINGS_ACCEPT_COOKIES _("IDS_BR_BODY_ACCEPT_COOKIES")
#define BR_STRING_SETTINGS_ACCEPT_COOKIES_DESC _("IDS_BR_BODY_ALLOWS_SITES_TO_SAVE_AND_READ_COOKIE_DATA")
#define BR_STRING_SETTINGS_ENABLE_LOCATION _("IDS_BR_BODY_ENABLE_LOCATION")
#define BR_STRING_SETTINGS_ENABLE_LOCATION_DESC _("IDS_BR_BODY_ALLOW_SITES_TO_REQUEST_ACCESS_TO_YOUR_LOCATION")
#define BR_STRING_SETTINGS_ENABLE_JAVASCRIPT _("IDS_BR_BODY_ENABLE_JAVASCRIPT")
#define BR_STRING_SETTINGS_ENABLE_JAVASCRIPT_DESC _("IDS_BR_BODY_ALLOW_SITES_TO_RUN_JAVASCRIPT")
#define BR_STRING_SETTINGS_BLOCK_POPUPS _("IDS_BR_BODY_BLOCK_POP_UPS")
#define BR_STRING_SETTINGS_BLOCK_POPUPS_DESC _("IDS_BR_BODY_BLOCK_POP_UPS_ON_WEB_PAGES")
#define BR_STRING_SETTINGS_BOOKMARK_REMINDER _("IDS_BR_BODY_BOOKMARK_REMINDER_ABB")
#define BR_STRING_SETTINGS_BOOKMARK_REMINDER_DESC _("IDS_BR_BODY_ASK_WHETHER_TO_ADD_A_BOOKMARK_FOR_A_WEBSITE_WHEN_YOU_HAVE_VISITED_IT_PD_TIMES")
#define BR_STRING_SETTINGS_DEFAULT_STORAGE _("IDS_BR_MBODY_DEFAULT_DOWNLOAD_STORAGE")
#define BR_STRING_SETTINGS_PHONE _("IDS_BR_OPT_DEVICE")
#define BR_STRING_SETTINGS_MEMORY_CARD _("IDS_BR_OPT_SD_CARD")
#define BR_STRING_SETTINGS_WEBSITE_SETTINGS _("IDS_BR_BODY_WEBSITE_SETTINGS")
#define BR_STRING_SETTINGS_WEBSITE_SETTINGS_DESC _("IDS_BR_BODY_ADVANCED_SETTINGS_FOR_INDIVIDUAL_WEBSITES")
#define BR_STRING_SETTINGS_WEB_NOTIFICATION_TITLE _("IDS_BR_HEADER_WEB_NOTIFICATION")
#define BR_STRING_SETTINGS_ENABLE_NOTIFICATIONS _("IDS_BR_BODY_NOTIFICATIONS")
#define BR_STRING_SETTINGS_ENABLE_NOTIFICATIONS_ALWAYS _("IDS_BR_BODY_ALWAYS_ON")
#define BR_STRING_SETTINGS_ENABLE_NOTIFICATIONS_ON_DEMAND _("IDS_BR_BODY_ON_DEMAND")
#define BR_STRING_SETTINGS_ENABLE_NOTIFICATIONS_OFF BR_STRING_OFF
#define BR_STRING_SETTINGS_CLEAR_NOTIFICATIONS _("IDS_BR_BODY_CLEAR_NOTIFICATIONS")
#define BR_STRING_SETTINGS_CLEAR_NOTIFICATIONS_DESC _("IDS_BR_BODY_CLEAR_NOTIFICATION_ACCESS_FOR_WEBSITES")
#define BR_STRING_SETTINGS_CLEAR_NOTIFICATIONS_Q _("IDS_BR_BODY_REMOVE_WEB_NOTIFICATION_ACCESS_INFORMATION_Q")

#define BR_STRING_SETTINGS_RESET_TO_DEFAULT _("IDS_BR_BODY_RESET_TO_DEFAULT")
#define BR_STRING_SETTINGS_RESET_TO_DEFAULT_DESC _("IDS_BR_BODY_CLEAR_ALL_BROWSER_DATA_AND_RESET_ALL_SETTINGS_TO_DEFAULT")///hard coding in po file about japanese.

#define BR_STRING_SETTINGS_PRELOAD_WEB_PAGES _("IDS_BR_MBODY_PRELOAD_WEBPAGES")
#define BR_STRING_SETTINGS_PRELOAD_WEB_PAGES_ALWAYS _("IDS_BR_BODY_ALWAYS")
#define BR_STRING_SETTINGS_PRELOAD_WEB_PAGES_ONLY_VIA_WIFI _("IDS_BR_BODY_ONLY_VIA_WI_FI_ABB")
#define BR_STRING_SETTINGS_PRELOAD_WEB_PAGES_NEVER BR_STRING_NEVER
#define BR_STRING_SETTINGS_LOAD_IMAGES _("IDS_BR_OPT_LOAD_IMAGES")
#define BR_STRING_SETTINGS_LOAD_IMAGES_DESC _("IDS_BR_BODY_DISPLAY_IMAGES_ON_WEB_PAGES")
#define BR_STRING_SETTINGS_OPEN_PAGES_IN_OVERVIEW _("IDS_BR_BODY_OPEN_PAGES_IN_OVERVIEW_ABB")
#define BR_STRING_SETTINGS_OPEN_PAGES_IN_OVERVIEW_DESC _("IDS_BR_BODY_SHOW_OVERVIEW_OF_NEWLY_OPENED_PAGES")

#define BR_STRING_SETTINGS_HTML5_EXTENSION _("IDS_BR_HEADER_HTML5_EXTENSION")
#define BR_STRING_SETTINGS_DRAG_AND_DROP _("IDS_BR_OPT_DRAG_AND_DROP")
#define BR_STRING_HTML5_FEATURE_WARNING_DESC _("IDS_BR_POP_SOME_OLD_HTML_WEBSITES_MAY_NOT_SUPPORT_THIS_FEATURE")

#define BR_STRING_SETTINGS_USER_AGENT _("IDS_BR_HEADER_USER_AGENT")
#define BR_STRING_SETTINGS_CUSTOM_USER_AGENT _("IDS_BR_BODY_CUSTOM_USER_AGENT")
#define BR_STRING_SETTINGS_WEB_INSPECTOR _("Web inspector")
/*************************************************************************************************************/


/* browser context menu */
/*************************************************************************************************************/
#define BR_STRING_CTXMENU_OPEN_LINK_IN_NEW_WINDOW       _("IDS_BR_OPT_OPEN_IN_NEW_WINDOW_ABB")
#define BR_STRING_CTXMENU_DOWNLOAD_LINKED_FILE          _("IDS_BR_BODY_DOWNLOAD")
#define BR_STRING_CTXMENU_COPY_LINK_URL                 _("IDS_BR_OPT_COPY_LINK_URL")
#define BR_STRING_CTXMENU_VIEW_IMAGE                    _("IDS_BR_BODY_VIEW_IMAGE")
#define BR_STRING_CTXMENU_OPEN_MEDIA_IN_NEW_WINDOW      _("IDS_BR_BODY_PLAY")
#define BR_STRING_CTXMENU_SAVE_IMAGE_AS                 _("Save image as")
#define BR_STRING_CTXMENU_COPY_IMAGE                    _("IDS_BR_OPT_COPY_IMAGE")
#define BR_STRING_CTXMENU_SAVE_IMAGE                    _("IDS_BR_OPT_SAVE_IMAGE")
#define BR_STRING_CTXMENU_SEND_IMAGE_VIA_EMAIL          _("IDS_BR_OPT_SEND_VIA_EMAIL")
#define BR_STRING_CTXMENU_SEND_IMAGE_VIA_MESSAGE        _("IDS_BR_OPT_SEND_VIA_MESSAGE")
#define BR_STRING_CTXMENU_COPY_VIDEO_LINK_LOCATION      _("IDS_BR_BODY_COPY_URL")
#define BR_STRING_CTXMENU_COPY_AUDIO_LINK_LOCATION      _("IDS_BR_BODY_COPY_URL")
#define BR_STRING_CTXMENU_COPY_TO_CLIPBOARD             _("IDS_BR_OPT_COPY_TO_CLIPBOARD")
#define BR_STRING_CTXMENU_FIND                          _("IDS_FV_BODY_FIND")
#define BR_STRING_CTXMENU_SAVE                          BR_STRING_SAVE
#define BR_STRING_CTXMENU_TRANSLATE                     _("IDS_BR_OPT_TRANSLATE")
#define BR_STRING_CTXMENU_PLAY                          _("IDS_BR_BODY_PLAY")
#define BR_STRING_CTXMENU_PAUSE                         dgettext("sys_string", "IDS_COM_BODY_PAUSE")
#define BR_STRING_CTXMENU_MUTE                          dgettext("sys_string", "IDS_COM_BODY_MUTE")
#define BR_STRING_CTXMENU_OPEN_FRAME_IN_NEW_WINDOW      _("Open frame in new window")
#define BR_STRING_CTXMENU_MORE                          _("IDS_BR_BUTTON_MORE")
#define BR_STRING_CTXMENU_COPY                          BR_STRING_COPY
#define BR_STRING_CTXMENU_WEB_SEARCH                    _("IDS_BR_OPT_WEB_SEARCH")
#define BR_STRING_CTXMENU_SHARE                         dgettext("sys_string", "IDS_COM_BUTTON_SHARE")
#define BR_STRING_CTXMENU_DELETE                        dgettext("sys_string", "IDS_COM_BODY_DELETE")
#define BR_STRING_CTXMENU_FIND_ON_PAGE                  _("IDS_BR_OPT_FIND_ON_PAGE")
#define BR_STRING_CTXMENU_SHARE_IMAGE_URL               _("Share image URL")
#define BR_STRING_CTXMENU_SHARE_LINK                    _("IDS_BR_BODY_SHARE_LINK")
#define BR_STRING_CTXMENU_SAVE_LINK                     _("IDS_BR_BODY_SAVE_LINK")
#define BR_STRING_CTXMENU_SELECT_ALL                    _("IDS_BR_OPT_SELECT_ALL")
#define BR_STRING_CTXMENU_SELECT                        _("IDS_BR_OPT_SELECT")
#define BR_STRING_CTXMENU_GO_BACK                       _("IDS_BR_OPT_NAVIGATE_GO_BACKWARD")
#define BR_STRING_CTXMENU_GO_FORWARD                    _("IDS_BR_OPT_NAVIGATE_GO_FORWARD")
#define BR_STRING_CTXMENU_STOP                          _("IDS_BR_OPT_STOP")
#define BR_STRING_CTXMENU_RELOAD                        _("IDS_BR_OPT_RELOAD")
#define BR_STRING_CTXMENU_CUT                           dgettext("sys_string", "IDS_COM_BODY_CUT")
#define BR_STRING_CTXMENU_PASTE                         _("IDS_BR_OPT_PASTE")
#define BR_STRING_CTXMENU_SELECT_WORD                   _("IDS_BR_BODY_SELECT_WORD")
#define BR_STRING_CTXMENU_DRAG                          BR_STRING_DRAG_AND_DROP
#define BR_STRING_CTXMENU_FONT                          _("Font")
#define BR_STRING_CTXMENU_CLOSE_ALL                     _("IDS_BR_OPT_CLOSE_ALL")
#define BR_STRING_CTXMENU_SELECTION_MODE                _("IDS_BR_OPT_SELECTION_MODE")
#define BR_STRING_CTXMENU_SAVE_TO_KEEPIT                _("IDS_BR_OPT_SAVE_TO_KEEPIT_ABB")
#define BR_STRING_CTXMENU_CALL                          BR_STRING_CALL
#define BR_STRING_CTXMENU_SEND_MESSAGE                  BR_STRING_SEND_MESSAGE
#define BR_STRING_CTXMENU_ADD_TO_CONTACT                BR_STRING_ADD_TO_CONTACT
#define BR_STRING_CTXMENU_SEND_EMAIL                    BR_STRING_SEND_EMAIL
#define BR_STRING_CTXMENU_GOOGLE                        _("Google")
#define BR_STRING_CTXMENU_YAHOO                         _("Yahoo")
#define BR_STRING_CTXMENU_BING                          _("Bing")
#define BR_STRING_CTXMENU_YANDEX                        _("Yandex")
/*************************************************************************************************************/

/* email, phone number handling */
/*************************************************************************************************************/
#define BR_STRING_SEND_EMAIL _("IDS_BR_OPT_SEND_EMAIL")
#define BR_STRING_SEND_MESSAGE _("IDS_BR_OPT_SEND_MESSAGE_VODA")
#define BR_STRING_ADD_TO_CONTACT _("IDS_BR_BODY_ADD_TO_CONTACT")
#define BR_STRING_COPY dgettext("sys_string", "IDS_COM_BODY_COPY")


/*************************************************************************************************************/

/* developer mode */
/*************************************************************************************************************/
#define BR_STRING_DEVELOPER_MODE                _("IDS_BR_BODY_DEVELOPER_MODE")
/*************************************************************************************************************/

/*newly added text*/
/*************************************************************************************************************/
#define BR_STRING_STORED_DATA	_("IDS_BR_BODY_STORED_DATA")
#define BR_STRING_SELECTED		_("IDS_BR_HEADER_PD_SELECTED_ABB")
#define BR_STRING_CLEAR_BROWSER_HISTORY_Q	_("IDS_BR_POP_CLEAR_BROWSER_HISTORY_Q");
#define BR_STRING_AFTER_YOU_LOAD_PAGES_THE_PAGE_HISTORY_WILL_BE_SHOWN		_("IDS_BR_BODY_AFTER_YOU_LOAD_PAGES_THE_PAGE_HISTORY_WILL_BE_SHOWN")
#define BR_STRING_NO_SAVED_PAGE		_("IDS_BR_POP_NO_SAVED_PAGE")
#define BR_STRING_THE_DESTINATION_FOLDER_IS_THE_SAME_AS_THE_SOURCE_FOLDER		_("IDS_BR_POP_THE_DESTINATION_FOLDER_IS_THE_SAME_AS_THE_SOURCE_FOLDER")
#define BR_STRING_THE_DESTINATION_FOLDER_IS_A_SUBFOLDER_OF_THE_SOURCE_FOLDER		_("IDS_BR_POP_THE_DESTINATION_FOLDER_IS_A_SUBFOLDER_OF_THE_SOURCE_FOLDER")
#define BR_STRING_ALL_OPEN_WINDOWS_WILL_BE_CLOSED_AND_YOU_WILL_MOVE_TO_YOUR_HOMEPAGE		_("IDS_BR_BODY_ALL_OPEN_WINDOWS_WILL_BE_CLOSED_AND_YOU_WILL_MOVE_TO_YOUR_HOMEPAGE")
#define BR_STRING_ADDED_TO_HOME_SCREEN		_("IDS_BR_TPOP_ADDED_TO_HOME_SCREEN")
#define BR_STRING_NO_MORE_ROOM_ON_HOME_SCREEN		_("IDS_BR_TPOP_NO_MORE_ROOM_ON_HOME_SCREEN_ABB")
#define BR_STRING_FAILED_TO_ADD_TO_HOME_SCREEN		_("IDS_BR_TPOP_FAILED_TO_ADD_TO_HOME_SCREEN_ABB")
#define BR_STRING_GOBACKWARD		_("IDS_BR_OPT_GOBACKWARD")
#define BR_STRING_GOFORWARD		_("IDS_BR_OPT_GOFORWARD")
#define BR_STRING_REFRESH		_("IDS_BR_OPT_REFRESH")
#define BR_STRING_CLEAR_ALL		_("IDS_BR_OPT_CLEAR_ALL")
#define BR_STRING_BODY_TITLE                _("IDS_BR_BODY_TITLE")
#define BR_STRING_VIEW_MORE	_("IDS_BR_MBODY_VIEW_MORE")
#define BR_STRING_TZTAB3_WINDOWS	_("IDS_BR_TZTAB3_WINDOWS")
#define BR_STRING_SAVE_TO_KEEPIT	_("IDS_BR_OPT_SAVE_TO_KEEPIT_ABB")
#define BR_STRING_FIND_ON_PAGE_ABB	_("IDS_BR_OPT_FIND_ON_PAGE_ABB")
#define BR_STRING_HELP	_("IDS_BR_OPT_HELP")
#define BR_STRING_MAX_CHARACTER_WARNING _("IDS_BR_BODY_THE_MAXIMUM_NUMBER_OF_CHARACTERS_HPD_HAS_BEEN_EXCEEDED")

/*TTS string*/
/*************************************************************************************************************/
#define BR_STRING_WEBPAGE_READER		_("IDS_BR_HEADER_READER")//have to change to TTS string
#define BR_STRING_DOUBLE_TAP_TO_OPEN_THE_FOLDER_T		_("IDS_BR_BODY_DOUBLE_TAP_TO_OPEN_THE_FOLDER_T_TTS")
#define BR_STRING_DOUBLE_TAP_TO_CLOSE_THE_TRAY_T		_("IDS_BR_BODY_DOUBLE_TAP_TO_CLOSE_THE_TRAY_T_TALKBACK")
#define BR_STRING_DOUBLE_TAP_AND_HOLD_TO_RESIZE_THE_SPLIT_AREA_T		_("IDS_BR_BODY_DOUBLE_TAP_AND_HOLD_TO_RESIZE_THE_SPLIT_AREA_T_TALKBACK")
#define BR_STRING_DOUBLE_TAP_TO_SELECT_A_SEARCH_ENGINE_T		_("IDS_BR_BODY_DOUBLE_TAP_TO_SELECT_A_SEARCH_ENGINE_T_TALKBACK")
#define BR_STRING_DOUBLE_TAP_TO_OPEN_THE_LIST_T		_("IDS_BR_BODY_DOUBLE_TAP_TO_OPEN_THE_LIST_T_TTS")
#define BR_STRING_OPEN_WINDOWS_T		_("IDS_BR_BODY_OPEN_WINDOWS_T_TALKBACK")
#define BR_STRING_VIEW_BOOKMARKS_T		_("IDS_BR_BODY_VIEW_BOOKMARKS_T_TALKBACK")
#define BR_STRING_VIEW_HISTORY_T		_("IDS_BR_BODY_VIEW_HISTORY_T_TALKBACK")
#define BR_STRING_ACCESS_DOUBLE_TAP_TO_OPEN_WEBPAGE_T	_("IDS_BR_BODY_DOUBLE_TAP_TO_OPEN_THE_WEBPAGE_T_TALKBACK")
#define BR_STRING_SEARCH_FIELD_T	_("IDS_BR_BODY_SEARCH_FIELD_T_TTS")
#define BR_STRING_CLICK_THIS_TO_LAUNCH_THE_APP	_("Click this to launch the app")
#define BR_STRING_UPPER_FOLDER_T_TALKBACK	_("IDS_BR_BODY_UPPER_FOLDER_T_TALKBACK")
#define BR_STRING_DOUBLE_TAP_TO_OPEN_KEYBOARD_T	_("IDS_BR_BODY_DOUBLE_TAP_TO_OPEN_KEYBOARD_T_TTS")
#define BR_STRING_DOUBLE_TAP_TO_MOVE_TO_CONTENT_T		_("IDS_BR_BODY_DOUBLE_TAP_TO_MOVE_TO_CONTENT_T_TTS")
#define BR_STRING_DOUBLE_TAP_AND_HOLD_TO_RESIZE_SPLIT_AREA_T		_("IDS_BR_BODY_DOUBLE_TAP_AND_HOLD_TO_RESIZE_SPLIT_AREA_T_TTS")
#define BR_STRING_TEXT_FIELD_T _("IDS_BR_BODY_TEXT_FIELD_T_TTS")
#define BR_STRING_DOUBLE_TAP_TO_CANCEL_THE_SEARCH_T_TALKBACK		_("IDS_BR_BODY_DOUBLE_TAP_TO_CANCEL_THE_SEARCH_T_TALKBACK")
//#define BR_STRING_CLEAR_TEXT_T		_("Clear text")
#define BR_STRING_ENLARGE_TEXT_T		_("IDS_BR_OPT_ENLARGE_TEXT")
#define BR_STRING_SHRINK_TEXT_T		_("IDS_BR_OPT_SHRINK_TEXT")
#define BR_STRING_WEBPAGE_LOADED_T			_("IDS_BR_BODY_PAGE_LOADED_T_TTS")
#define BR_STRING_BUTTON_T		_("IDS_COM_BODY_BUTTON_T_TTS")
#define BR_STRING_OPEN_APPLICATION_T		_("IDS_SCR_BODY_OPEN_APPLICATION")
/*accessability*/
/*************************************************************************************************************/
//#define BR_STRING_BACK          dgettext("sys_string", "IDS_COM_BODY_BACK")
//#define BR_STRING_CLOSE_WINDOW          _("IDS_BR_OPT_CLOSE_WINDOW")
#define BR_STRING_ACCESS_PREV_BUT                _("IDS_BR_SK_PREVIOUS")
#define BR_STRING_ACCESS_NEXT_BUT                _("IDS_BR_SK_NEXT")
#define BR_STRING_ACCESS_MENU_BUT                _("IDS_BR_SK_MENU")
//#define BR_STRING_CTXMENU_MORE
/*************************************************************************************************************/

//below string exist already. using it. search with IDS_~.
//#define BR_STRING_DEFAULT_PAGE		_("IDS_BR_BODY_DEFAULT_PAGE_ABB")

//below string have to request. It only has ss_id.
//IDS_COM_BODY_SEARCH_FIELD_T_TTS
//SS_DOUBLE_TAP_TO_OPEN_KEYBOARD_T_TTS
//IDS_TPLATFORM_BODY_DOUBLE_TAP_TO_MOVE_TO_CONTENT_T_TTS
//IDS_COM_BODY_DRAG_SCROLL_T_TTS
//IDS_MSGS_BODY_EXPANDABLE_LIST_T_TTS
//IDS_COM_BODY_TEXT_FIELD_T_TTS

/*Help UI string*/
#define BR_STRING_PRESS_THE_MENU_KEY                _("IDS_BR_BODY_PRESS_THE_MENU_KEY")
//#define BR_STRING_ADDING_BOOKMARKS                  _("IDS_HELP_HEADER_ADDING_BOOKMARKS_ABB")
//#define BR_STRING_OPENING_NEW_WINDOWS               _("IDS_HELP_HEADER_OPENING_NEW_WINDOWS_ABB")
//#define BR_STRING_USING_THE_SEARCH_BAR              _("IDS_HELP_HEADER_USING_THE_SEARCH_BAR_ABB")
//#define BR_STRING_CHANGING_THE_SEARCH_ENGINE        _("IDS_HELP_HEADER_CHANGING_THE_SEARCH_ENGINE_ABB")
//#define BR_STRING_ADDING_BOOKMARK_DETAIL            _("IDS_HELP_BODY_YOU_CAN_ADD_WEBSITES_YOU_VISIT_FREQUENTLY_TO_YOUR_BOOKMARKS_TO_OPEN_THEM_FASTER")
//#define BR_STRING_TAP_ADD_BOOKMARK_TO_ADD_A_BOOKMARK        _("IDS_HELP_BODY_TAP_ADD_BOOKMARK_TO_ADD_A_BOOKMARK")
#define BR_STRING_EDIT_THE_TITLE_AND_LOCATION_THEN_TAP_SAVE        _("IDS_HELP_BODY_EDIT_THE_TITLE_AND_LOCATION_THEN_TAP_SAVE")
//#define BR_STRING_OPENING_NEW_WINDOWS_DETAIL        _("IDS_HELP_BODY_YOU_CAN_OPEN_MULTIPLE_WINDOWS_TO_VISIT_DIFFERENT_WEBSITES_AT_THE_SAME_TIME")
#define BR_STRING_TAP_THE_WINDOWS_ICON        _("IDS_HELP_TPOP_TAP_THE_WINDOWS_ICON")
#define BR_STRING_TAP_TO_ADD_NEW_WINDOW        _("IDS_HELP_POP_TAP_TO_ADD_NEW_WINDOW")
//#define BR_STRING_USING_THE_SEARCH_BAR_DETAIL        _("IDS_HELP_BODY_YOU_CAN_NAVIGATE_TO_OTHER_WEBSITES_AND_SEARCH_THE_INTERNET_USING_THE_URL_BAR")
#define BR_STRING_TAP_URL_BAR_TO_CHANGE_TO_SEARCH_MODE        _("IDS_HELP_BODY_TAP_URL_BAR_TO_CHANGE_TO_SEARCH_MODE")
#define BR_STRING_TAP_TO_DELETE_ALL_TEXT        _("IDS_HELP_BODY_TAP_TO_DELETE_ALL_TEXT")
#define BR_STRING_ENTER_KEYWORD_OR_URL        _("IDS_HELP_BODY_ENTER_KEYWORD_OR_URL")
#define BR_STRING_TAP_TO_CHANGE_THE_SEARCH_ENGINE        _("IDS_HELP_BODY_TAP_TO_CHANGE_THE_SEARCH_ENGINE")
#define BR_STRING_SELECT_A_SEARCH_ENGINE_FROM_THE_LIST        _("IDS_HELP_BODY_SELECT_A_SEARCH_ENGINE_FROM_THE_LIST")
//#define BR_STRING_HELP_INTERNET        _("IDS_HELP_BODY_INTERNET")
//#define BR_STRING_BROWSE_WEBPAGES_AND_ADD_BOOKMARKS        _("IDS_HELP_BODY_BROWSE_WEBPAGES_AND_ADD_BOOKMARKS")
//#define BR_STRING_CHANGING_THE_SEARCH_ENGINE_DETAIL        _("IDS_HELP_BODY_YOU_CAN_SELECT_THE_SEARCH_ENGINE_YOU_WANT_TO_USE_BEFORE_STARTING_A_SEARCH")
//#define BR_STRING_TAB_THE_BOOKMARK_TAB        _("IDS_HELP_BODY_TAP_THE_BOOKMARK_TAB")
#define BR_STRING_TUTORIAL_COMPLETE        _("IDS_HELP_POP_TUTORIAL_COMPLETE")
#define BR_STRING_TAP_TO_ADD_BOOKMARK        _("IDS_HELP_BODY_TAP_TO_ADD_BOOKMARK_ABB")
#define BR_STRING_INVALID_ACTION	_("IDS_HELP_POP_INVALID_ACTION_TRY_AGAIN")
/*************************************************************************************************************/

/*certificate string*/
#define BR_STRING_UNABLE_TO_VIEW_THE_CERTIFICATE_THE_PAGE_INFORMATION_HAS_BEEN_CHANGED	_("IDS_BR_POP_UNABLE_TO_VIEW_THE_CERTIFICATE_THE_PAGE_INFORMATION_HAS_BEEN_CHANGED")
#define BR_STRING_ISSUED_TO_C		_("IDS_BR_HEADER_ISSUED_TO_C")
#define BR_STRING_CERTIFICATE_SERIAL_NUMBER		_("IDS_BR_BODY_CERTIFICATE_SERIAL_NUMBER")
#define BR_STRING_ISSUED_BY_C		_("IDS_BR_HEADER_ISSUED_BY_C")
#define BR_STRING_VALIDITY_C		_("IDS_BR_HEADER_VALIDITY_C")
#define BR_STRING_EXPIRES_ON_C		_("IDS_BR_HEADER_EXPIRES_ON_C")
#define BR_STRING_SECURITY_CERTIFICATE		_("IDS_BR_HEADER_SECURITY_CERTIFICATE")
#define BR_STRING_VIEW_CERTIFICATE		_("IDS_BR_OPT_VIEW_CERTIFICATE")

/* Design ID is using instead of browser string marco */
//#define BR_STRING_DELETE_THE_SELECTED_BOOKMARKS_Q _("IDS_BR_BODY_DELETE_THE_SELECTED_BOOKMARKS_Q")

/*newly added string*/
#define BR_STRING_CERTIFICATE		_("IDS_ST_HEADER_CERTIFICATE")
#define BR_STRING_SPLIT_BAR_HANDLE		_("IDS_BR_BODY_SPLIT_BAR_HANDLE")
#define BR_STRING_ADD_OR_REMOVED_BOOKMARK		_("IDS_BR_OPT_ADD_OR_REMOVE_BOOKMARK")
#define BR_STRING_ADDED                        dgettext("sys_string", "IDS_COM_OPT_ADDED")
#define BR_STRING_REMOVED		_("IDS_BR_POP_REMOVED")
#define BR_STRING_VALIDITY		_("IDS_BR_HEADER_VALIDITY_C")
#define BR_STRING_ISSUED_ON		_("IDS_BR_HEADER_ISSUED_ON_C")
#define BR_STRING_EXPIRED_ON		_("IDS_BR_HEADER_EXPIRES_ON_C")
#define BR_STRING_FINGERPRINTS		_("IDS_BR_BODY_FINGERPRINTS_C")
#define BR_STRING_FINGERPRINTS_SHA256     _("IDS_BR_BODY_SHA_256_FINGERPRINT_C")
#define BR_STRING_FINGERPRINTS_SHA1     _("IDS_BR_BODY_SHA_1_FINGERPRINT_C")
#define BR_STRING_ORGANIZATION		_("IDS_BR_HEADER_ORGANIZATION_C")
#define BR_STRING_ORGANIZATION_UNIT		_("IDS_BR_HEADER_ORGANISATIONAL_UNIT_C")
#define BR_STRING_VALID_CERTIFICATE		_("Valid Certificate")
#define BR_STRING_DISK_FULL		_("IDS_COM_BODY_NOT_ENOUGH_MEMORY_DELETE_SOME_ITEMS_AND_TRY_AGAIN")
#define BR_STRING_NO_WINDOWS		_("IDS_BR_BODY_NO_WINDOWS")
#endif /* BROWSER_STRING_H */
