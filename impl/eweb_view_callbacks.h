/*
    Copyright (C) 2014 Samsung Electronics

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

#ifndef EWEB_VIEW_CALLBACKS
#define EWEB_VIEW_CALLBACKS

#include "eweb_view.h"
#include "API/ewk_console_message_private.h"
#include "API/ewk_error_private.h"
#include "API/ewk_context_menu_private.h"
#include "API/ewk_auth_challenge_private.h"
#include "API/ewk_text_style_private.h"
#include "API/ewk_geolocation_private.h"
#include "API/ewk_certificate_private.h"
#include "API/ewk_user_media_private.h"
#include "tizen_webview/public/tw_custom_handlers.h"
#include "tizen_webview/public/tw_policy_decision.h"

#include <Evas.h>
#include <Eina.h>

typedef struct EwkObject Ewk_Auth_Request;
typedef struct EwkObject Ewk_Download_Job;
typedef struct EwkObject Ewk_File_Chooser_Request;
typedef struct EwkObject Ewk_Form_Submission_Request;
typedef struct EwkObject Ewk_Navigation_Policy_Decision;

namespace tizen_webview {
class Notification;
class NotificationPermissionRequest;
class WrtIpcMessageData;
}

namespace EWebViewCallbacks {

enum CallbackType {
  AuthenticationRequest,
  BackForwardListChange,
  CancelVibration,
  CreateNewWindow,
  ContentsSizeChanged,
  DownloadJobCancelled,
  DownloadJobFailed,
  DownloadJobFinished,
  DownloadJobRequested,
  FileChooserRequest,
  NewFormSubmissionRequest,
  FaviconChanged,
  LoadError,
  LoadStarted,
  LoadFinished,
  LoadProgress,
  LoadProgressStarted,
  LoadProgressFinished,
  MenuBarVisible,
  ProvisionalLoadFailed,
  ProvisionalLoadRedirect,
  ProvisionalLoadStarted,
  LoadCommitted,
  StatusBarVisible,
  NavigationPolicyDecision,
  NewWindowPolicyDecision,
  TextFound,
  TitleChange,
  ToolbarVisible,
  TooltipTextUnset,
  TooltipTextSet,
  URLChanged,
  Vibrate,
  WebProcessCrashed,
  WindowClosed,
  WindowResizable,
  EnterFullscreen,
  ExitFullscreen,
  UserMediaPermission,
  RegisterProtocolHandler,
  IMEInputPanelShow,
  IMEInputPanelHide,
  IMECandidatePanelShow,
  IMECandidatePanelHide,
  IMEInputMethodChanged,
  GeoLocationPermissionRequest,
  RequestCertificateConfirm,
  AuthChallenge,
  NotificationShow,
  NotificationCancel,
  NotificationPermissionRequest,
  PolicyResponseDecide,
  ContextMenuCustomize,
  ContextMenuItemSelected,
  LoadNonEmptyLayoutFinished,
  PopupReplyWaitStart,
  PopupReplyWaitFinish,
  FrameRendered,
  EdgeLeft,
  EdgeRight,
  EdgeTop,
  EdgeBottom,
  TextStyleState,
  ScrollLeft,
  ScrollRight,
  ScrollUp,
  ScrollDown,
  UndoSize,
  RedoSize,
  MagnifierShow,
  MagnifierHide,
  ClipboardOpened,
  ConsoleMessage,
  WrtPluginsMessage,
  IconReceived,
  FormSubmit,
  OverflowScrollOff,
  OverflowScrollOn,
  TouchmoveHandled,
  WebloginCheckboxClicked,
  WebloginCheckboxResume,
  WebloginReady,
#if defined(OS_TIZEN_TV)
  NewWindowNavigationPolicyDecision,
#endif // OS_TIZEN_TV
  URIChanged
};

template <CallbackType>
struct CallBackInfo;

class EvasObjectHolder {
protected:
  explicit EvasObjectHolder(Evas_Object* object)
      : m_object(object)
  {
  }

  Evas_Object* m_object;
};

template <CallbackType callbackType, typename ArgType = typename CallBackInfo<callbackType>::Type>
struct CallBack: public EvasObjectHolder {
  explicit CallBack(Evas_Object* view) : EvasObjectHolder(view) { }

  void call(ArgType argument) {
    evas_object_smart_callback_call(m_object, CallBackInfo<callbackType>::name(), static_cast<void*>(argument));
  }
};

template <CallbackType callbackType>
struct CallBack <callbackType, void> : public EvasObjectHolder {
  explicit CallBack(Evas_Object* view) : EvasObjectHolder(view) { }

  void call() {
    evas_object_smart_callback_call(m_object, CallBackInfo<callbackType>::name(), 0);
  }
};

template <CallbackType callbackType>
struct CallBack <callbackType, const char*> : public EvasObjectHolder {
  explicit CallBack(Evas_Object* view) : EvasObjectHolder(view) { }

  void call(const char* arg) {
    evas_object_smart_callback_call(m_object, CallBackInfo<callbackType>::name(), const_cast<char*>(arg));
  }
};

// TODO: convert for our needs.
#if 0
template <CallbackType callbackType>
struct CallBack <callbackType, Ewk_CSS_Size*> : public EvasObjectHolder {
  explicit CallBack(Evas_Object* view) : EvasObjectHolder(view) { }

  void call(Ewk_CSS_Size* size)
  {
    evas_object_smart_callback_call(m_object, CallBackInfo<callbackType>::name(), size);
  }

  void call(const WKSize& arg)
  {
    Ewk_CSS_Size size = { static_cast<Evas_Coord>(arg.width), static_cast<Evas_Coord>(arg.height) };
    call(&size);
  }
};
#endif

#define DECLARE_EWK_VIEW_CALLBACK(callbackType, string, type) \
template <>                                                   \
struct CallBackInfo<callbackType> {                           \
  typedef type Type;                                          \
  static inline const char* name() { return string; }         \
}

// Copied from WebKit as is. TODO: implement each and delete this section.
/*
DECLARE_EWK_VIEW_CALLBACK(AuthenticationRequest, "authentication,request", Ewk_Auth_Request*);
DECLARE_EWK_VIEW_CALLBACK(CancelVibration, "cancel,vibration", void);
DECLARE_EWK_VIEW_CALLBACK(ContentsSizeChanged, "contents,size,changed", Ewk_CSS_Size*);
DECLARE_EWK_VIEW_CALLBACK(DownloadJobCancelled, "download,cancelled", Ewk_Download_Job*);
DECLARE_EWK_VIEW_CALLBACK(DownloadJobFailed, "download,failed", Ewk_Download_Job_Error*);
DECLARE_EWK_VIEW_CALLBACK(DownloadJobFinished, "download,finished", Ewk_Download_Job*);
DECLARE_EWK_VIEW_CALLBACK(DownloadJobRequested, "download,request", Ewk_Download_Job*);
DECLARE_EWK_VIEW_CALLBACK(NewFormSubmissionRequest, "form,submission,request", Ewk_Form_Submission_Request*);
DECLARE_EWK_VIEW_CALLBACK(FaviconChanged, "favicon,changed", void);
DECLARE_EWK_VIEW_CALLBACK(LoadError, "load,error", Ewk_Error*);
DECLARE_EWK_VIEW_CALLBACK(ProvisionalLoadFailed, "load,provisional,failed", Ewk_Error*);
DECLARE_EWK_VIEW_CALLBACK(NavigationPolicyDecision, "policy,decision,navigation", Ewk_Navigation_Policy_Decision*);
DECLARE_EWK_VIEW_CALLBACK(TextFound, "text,found", unsigned*);
DECLARE_EWK_VIEW_CALLBACK(TooltipTextUnset, "tooltip,text,unset", void);
DECLARE_EWK_VIEW_CALLBACK(Vibrate, "vibrate", uint32_t*);
*/

// Note: type 'void' means that no arguments are expected.

DECLARE_EWK_VIEW_CALLBACK(FileChooserRequest, "file,chooser,request", Ewk_File_Chooser_Request*);
DECLARE_EWK_VIEW_CALLBACK(FormSubmit, "form,submit", const char*);
DECLARE_EWK_VIEW_CALLBACK(LoadFinished, "load,finished", void);
DECLARE_EWK_VIEW_CALLBACK(LoadStarted, "load,started", void);
DECLARE_EWK_VIEW_CALLBACK(LoadError, "load,error", _Ewk_Error*);
DECLARE_EWK_VIEW_CALLBACK(TitleChange, "title,changed", const char*);
DECLARE_EWK_VIEW_CALLBACK(URLChanged, "url,changed", const char*);
DECLARE_EWK_VIEW_CALLBACK(URIChanged, "uri,changed", const char*);
DECLARE_EWK_VIEW_CALLBACK(LoadProgress, "load,progress", double*);
DECLARE_EWK_VIEW_CALLBACK(LoadProgressStarted, "load,progress,started", void);
DECLARE_EWK_VIEW_CALLBACK(LoadProgressFinished, "load,progress,finished", void);
DECLARE_EWK_VIEW_CALLBACK(TooltipTextSet, "tooltip,text,set", const char*);
DECLARE_EWK_VIEW_CALLBACK(EnterFullscreen, "fullscreen,enterfullscreen", void);
DECLARE_EWK_VIEW_CALLBACK(ExitFullscreen, "fullscreen,exitfullscreen", void);
DECLARE_EWK_VIEW_CALLBACK(UserMediaPermission, "usermedia,permission,request", _Ewk_User_Media_Permission_Request*);
DECLARE_EWK_VIEW_CALLBACK(RegisterProtocolHandler, "protocolhandler,registration,requested", tizen_webview::Custom_Handlers_Data*);
DECLARE_EWK_VIEW_CALLBACK(IMEInputPanelShow, "editorclient,ime,opened", void);
DECLARE_EWK_VIEW_CALLBACK(IMEInputPanelHide, "editorclient,ime,closed", void);
DECLARE_EWK_VIEW_CALLBACK(IMECandidatePanelShow, "editorclient,candidate,opened", void);
DECLARE_EWK_VIEW_CALLBACK(IMECandidatePanelHide, "editorclient,candidate,closed", void);
DECLARE_EWK_VIEW_CALLBACK(CreateNewWindow, "create,window", Evas_Object**);
DECLARE_EWK_VIEW_CALLBACK(WindowClosed, "close,window", void);
DECLARE_EWK_VIEW_CALLBACK(IMEInputMethodChanged, "inputmethod,changed", Eina_Rectangle*);
DECLARE_EWK_VIEW_CALLBACK(ProvisionalLoadStarted, "load,provisional,started", void);
DECLARE_EWK_VIEW_CALLBACK(ProvisionalLoadRedirect, "load,provisional,redirect", void);
DECLARE_EWK_VIEW_CALLBACK(LoadCommitted, "load,committed", void);
DECLARE_EWK_VIEW_CALLBACK(GeoLocationPermissionRequest, "geolocation,permission,request", _Ewk_Geolocation_Permission_Request*);
DECLARE_EWK_VIEW_CALLBACK(RequestCertificateConfirm, "request,certificate,confirm", _Ewk_Certificate_Policy_Decision*);
DECLARE_EWK_VIEW_CALLBACK(AuthChallenge, "authentication,challenge", _Ewk_Auth_Challenge*);
DECLARE_EWK_VIEW_CALLBACK(NotificationShow, "notification,show", tizen_webview::Notification*);
DECLARE_EWK_VIEW_CALLBACK(NotificationCancel, "notification,cancel", uint64_t*);
DECLARE_EWK_VIEW_CALLBACK(NotificationPermissionRequest, "notification,permission,request", tizen_webview::NotificationPermissionRequest*);
DECLARE_EWK_VIEW_CALLBACK(PolicyResponseDecide, "policy,response,decide", tizen_webview::PolicyDecision*);
DECLARE_EWK_VIEW_CALLBACK(ContextMenuCustomize, "contextmenu,customize", _Ewk_Context_Menu*);
DECLARE_EWK_VIEW_CALLBACK(ContextMenuItemSelected, "contextmenu,selected", _Ewk_Context_Menu_Item*);
DECLARE_EWK_VIEW_CALLBACK(NavigationPolicyDecision, "policy,navigation,decide", tizen_webview::PolicyDecision*);
DECLARE_EWK_VIEW_CALLBACK(TextFound, "text,found", unsigned int*);
DECLARE_EWK_VIEW_CALLBACK(NewWindowPolicyDecision, "policy,newwindow,decide", tizen_webview::PolicyDecision*);
DECLARE_EWK_VIEW_CALLBACK(LoadNonEmptyLayoutFinished, "load,nonemptylayout,finished", void);
DECLARE_EWK_VIEW_CALLBACK(PopupReplyWaitStart, "popup,reply,wait,start", void*);
DECLARE_EWK_VIEW_CALLBACK(PopupReplyWaitFinish, "popup,reply,wait,finish", void*);
DECLARE_EWK_VIEW_CALLBACK(FrameRendered, "frame,rendered", void*);
DECLARE_EWK_VIEW_CALLBACK(EdgeLeft, "edge,left", void);
DECLARE_EWK_VIEW_CALLBACK(EdgeTop, "edge,top", void);
DECLARE_EWK_VIEW_CALLBACK(EdgeBottom, "edge,bottom", void);
DECLARE_EWK_VIEW_CALLBACK(EdgeRight, "edge,right", void);
DECLARE_EWK_VIEW_CALLBACK(TextStyleState, "text,style,state", _Ewk_Text_Style*);
DECLARE_EWK_VIEW_CALLBACK(ScrollRight, "scroll,right", void);
DECLARE_EWK_VIEW_CALLBACK(ScrollLeft, "scroll,left", void);
DECLARE_EWK_VIEW_CALLBACK(ScrollUp, "scroll,up", void);
DECLARE_EWK_VIEW_CALLBACK(ScrollDown, "scroll,down", void);
DECLARE_EWK_VIEW_CALLBACK(UndoSize, "undo,size", size_t*);
DECLARE_EWK_VIEW_CALLBACK(RedoSize, "redo,size", size_t*);
DECLARE_EWK_VIEW_CALLBACK(MagnifierShow, "magnifier,show", void);
DECLARE_EWK_VIEW_CALLBACK(MagnifierHide, "magnifier,hide", void);
DECLARE_EWK_VIEW_CALLBACK(ClipboardOpened, "clipboard,opened", void*);
DECLARE_EWK_VIEW_CALLBACK(BackForwardListChange, "back,forward,list,changed", void);
DECLARE_EWK_VIEW_CALLBACK(WebProcessCrashed, "webprocess,crashed", bool*);
DECLARE_EWK_VIEW_CALLBACK(ConsoleMessage, "console,message", _Ewk_Console_Message*);
DECLARE_EWK_VIEW_CALLBACK(WrtPluginsMessage, "wrt,message", tizen_webview::WrtIpcMessageData*);
DECLARE_EWK_VIEW_CALLBACK(IconReceived, "icon,received", void);
DECLARE_EWK_VIEW_CALLBACK(OverflowScrollOff, "overflow,scroll,off", void);
DECLARE_EWK_VIEW_CALLBACK(OverflowScrollOn, "overflow,scroll,on", void);
DECLARE_EWK_VIEW_CALLBACK(TouchmoveHandled, "touchmove,handled", bool);
DECLARE_EWK_VIEW_CALLBACK(WebloginCheckboxClicked, "weblogin,checkbox,clicked", void);
DECLARE_EWK_VIEW_CALLBACK(WebloginCheckboxResume, "weblogin,checkbox,resume", void);
DECLARE_EWK_VIEW_CALLBACK(WebloginReady, "weblogin,ready" , void);
#if defined(OS_TIZEN_TV)
DECLARE_EWK_VIEW_CALLBACK(NewWindowNavigationPolicyDecision, "policy,decision,new,window", Ewk_Navigation_Policy_Decision*);
#endif // OS_TIZEN_TV

DECLARE_EWK_VIEW_CALLBACK(MenuBarVisible, "menubar,visible", bool*);
DECLARE_EWK_VIEW_CALLBACK(StatusBarVisible, "statusbar,visible", bool*);
DECLARE_EWK_VIEW_CALLBACK(ToolbarVisible, "toolbar,visible", bool*);
DECLARE_EWK_VIEW_CALLBACK(WindowResizable, "window,resizable", bool*);

}

#endif
