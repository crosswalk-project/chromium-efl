#include "base/strings/utf_string_conversions.h"
#include "browser/javascript_modal_dialog_efl.h"
#include "eweb_view.h"
#include "base/path_service.h"
#include "base/files/file_path.h"
#include "paths_efl.h"

#ifdef OS_TIZEN_MOBILE
#include <dlfcn.h>
#include <efl_assist.h>
extern void* EflAssistHandle;
#endif

//static
JavaScriptModalDialogEfl* JavaScriptModalDialogEfl::CreateDialog(content::WebContents* web_contents,
                           const GURL& origin_url,
                           const std::string& accept_lang,
                           content::JavaScriptMessageType javascript_message_type,
                           const base::string16& message_text,
                           const base::string16& default_prompt_text,
                           const content::JavaScriptDialogManager::DialogClosedCallback& callback) {
  JavaScriptModalDialogEfl* dialog = new JavaScriptModalDialogEfl(web_contents,
      origin_url, accept_lang, javascript_message_type, message_text, default_prompt_text, callback);
  if (!dialog->ShowJavaScriptDialog()) {
    delete dialog;
    dialog = NULL;
  }
  return dialog;
}

JavaScriptModalDialogEfl::JavaScriptModalDialogEfl(content::WebContents* web_contents,
                                                   const GURL& origin_url,
                                                   const std::string& accept_lang,
                                                   content::JavaScriptMessageType javascript_message_type,
                                                   const base::string16& message_text,
                                                   const base::string16& default_prompt_text,
                                                   const content::JavaScriptDialogManager::DialogClosedCallback& callback)
    : callback_(callback)
    , origin_url_(origin_url)
    , accept_lang_(accept_lang)
    , javascript_message_type_(javascript_message_type)
    , message_text_(message_text)
    , default_prompt_text_(default_prompt_text)
    , web_contents_delegate_(NULL)
    , window_(NULL)
    , prompt_entry_(NULL)
    , ok_button_(NULL)
    , cancel_button_(NULL)
    , webview_(NULL)
    , widgetWin_(NULL)
    , imfContext(NULL) {
  DCHECK(web_contents);
  if(NULL != web_contents) {
    web_contents_delegate_ = static_cast<content::WebContentsDelegateEfl*>(web_contents->GetDelegate());
  }
  if(NULL != web_contents_delegate_) {
    webview_ = web_contents_delegate_->web_view()->evas_object();
  }
}

static void promptEntryChanged(void* data, Ecore_IMF_Context* ctx, int value) {
  if (value == ECORE_IMF_INPUT_PANEL_STATE_HIDE) {
    Evas_Object* entry = static_cast<Evas_Object*>(data);
    if(entry)
      elm_object_focus_set(entry, EINA_FALSE);
  }
}

static void promptEnterKeyDownCallback(void* data, Evas_Object* obj, void* eventInfo) {
  elm_entry_input_panel_hide(obj);
}

bool JavaScriptModalDialogEfl::ShowJavaScriptDialog() {
  popup_ = popupAdd();

  if (!popup_)
    return false;

  if (javascript_message_type_ == content::JAVASCRIPT_MESSAGE_TYPE_PROMPT) {
    if (message_text_.c_str())
      elm_object_part_text_set(popup_, "title,text", UTF16ToUTF8(message_text_).c_str());

    base::FilePath edj_dir;
    base::FilePath javaScriptPopup_edj;
    PathService::Get(PathsEfl::EDJE_RESOURCE_DIR, &edj_dir);
    javaScriptPopup_edj = edj_dir.Append(FILE_PATH_LITERAL("JavaScriptPopup.edj"));

    Evas_Object* layout = elm_layout_add(popup_);
    elm_layout_file_set(layout, javaScriptPopup_edj.AsUTF8Unsafe().c_str(), "prompt");
    evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

    prompt_entry_ = elm_entry_add(popup_);

    imfContext = static_cast<Ecore_IMF_Context*>(elm_entry_imf_context_get(prompt_entry_));
    ecore_imf_context_input_panel_event_callback_add(imfContext, ECORE_IMF_INPUT_PANEL_STATE_EVENT, promptEntryChanged, prompt_entry_);
    elm_entry_single_line_set(prompt_entry_, EINA_TRUE);
    elm_entry_input_panel_return_key_type_set(prompt_entry_, ELM_INPUT_PANEL_RETURN_KEY_TYPE_DONE );
    evas_object_smart_callback_add(prompt_entry_, "activated", promptEnterKeyDownCallback, this);
    elm_object_text_set(prompt_entry_, UTF16ToUTF8(default_prompt_text_).c_str());
    elm_entry_cursor_end_set(prompt_entry_);

    elm_object_part_content_set(layout, "prompt_container", prompt_entry_);
    elm_object_content_set(popup_, layout);

    cancel_button_ = elm_button_add(popup_);
    elm_object_style_set(cancel_button_, "popup");
    elm_object_text_set(cancel_button_, "Cancel");
    elm_object_part_content_set(popup_, "button1", cancel_button_);
    evas_object_smart_callback_add(cancel_button_, "clicked", CancelButtonHandlerForPrompt, this);

    ok_button_ = elm_button_add(popup_);
    elm_object_style_set(ok_button_, "popup");
    elm_object_text_set(ok_button_, "OK");
    elm_object_part_content_set(popup_, "button2", ok_button_);
    evas_object_focus_set(ok_button_, true);
    evas_object_smart_callback_add(ok_button_, "clicked", OkButtonHandlerForPrompt, this);

#if !defined(EWK_BRINGUP)
  } else if (javascript_message_type_ == content::JAVASCRIPT_MESSAGE_TYPE_NAVIGATION_PROMPT) {
    if (message_text_.c_str())
      elm_object_part_text_set(popup_, "title,text", UTF16ToUTF8(message_text_).c_str());

    std::string message;
    if (default_prompt_text_.c_str())
      message = std::string(UTF16ToUTF8(default_prompt_text_).c_str()) + std::string("\nLeave this page?");
    else
      message = std::string("Leave this page?");

    setLabelText(message.c_str());

    cancel_button_ = elm_button_add(popup_);
    elm_object_style_set(cancel_button_, "popup");
    elm_object_text_set(cancel_button_, "Leave");
    elm_object_part_content_set(popup_, "button1", cancel_button_);
    evas_object_smart_callback_add(cancel_button_, "clicked", OkButtonHandlerForPrompt, this);

    ok_button_ = elm_button_add(popup_);
    elm_object_style_set(ok_button_, "popup");
    elm_object_text_set(ok_button_, "Stay");
    elm_object_part_content_set(popup_, "button2", ok_button_);
    evas_object_smart_callback_add(ok_button_, "clicked", CancelButtonHandlerForPrompt, this);
#endif
  } else if (javascript_message_type_ == content::JAVASCRIPT_MESSAGE_TYPE_ALERT) {
    if (!setLabelText(UTF16ToUTF8(message_text_).c_str()))
      return false;

    ok_button_ = elm_button_add(popup_);
    elm_object_style_set(ok_button_, "popup");
    elm_object_text_set(ok_button_, "OK");
    elm_object_part_content_set(popup_, "button1", ok_button_);

    evas_object_smart_callback_add(ok_button_, "clicked", OkButtonHandlerForAlert, this);
  } else if(javascript_message_type_ == content::JAVASCRIPT_MESSAGE_TYPE_CONFIRM) {
    if (!setLabelText(UTF16ToUTF8(message_text_).c_str()))
      return false;

    cancel_button_ = elm_button_add(popup_);
    elm_object_style_set(cancel_button_, "popup");
    elm_object_text_set(cancel_button_, "Cancel");
    elm_object_part_content_set(popup_, "button1", cancel_button_);
    evas_object_smart_callback_add(cancel_button_, "clicked", CancelButtonHandlerForConfirm, this);

    ok_button_ = elm_button_add(popup_);
    elm_object_style_set(ok_button_, "popup");
    elm_object_text_set(ok_button_, "OK");
    elm_object_part_content_set(popup_, "button2", ok_button_);
    evas_object_smart_callback_add(ok_button_, "clicked", OkButtonHandlerForConfirm, this);
  }

#ifdef OS_TIZEN_MOBILE
  if (EflAssistHandle) {
    void (*webkit_ea_object_event_callback_add)(Evas_Object *, Ea_Callback_Type , Ea_Event_Cb func, void *);
    webkit_ea_object_event_callback_add = (void (*)(Evas_Object *, Ea_Callback_Type , Ea_Event_Cb func, void *))dlsym(EflAssistHandle, "ea_object_event_callback_add");
#if !defined(EWK_BRINGUP)
    if(javascript_message_type_ == content::JAVASCRIPT_MESSAGE_TYPE_PROMPT || javascript_message_type_ == content::JAVASCRIPT_MESSAGE_TYPE_NAVIGATION_PROMPT)
      (*webkit_ea_object_event_callback_add)(popup_, EA_CALLBACK_BACK, CancelButtonHandlerForPrompt, this);
    else 
#endif
    if(javascript_message_type_ == content::JAVASCRIPT_MESSAGE_TYPE_ALERT)
      (*webkit_ea_object_event_callback_add)(popup_, EA_CALLBACK_BACK, CancelButtonHandlerForAlert, this);
    else
      (*webkit_ea_object_event_callback_add)(popup_, EA_CALLBACK_BACK, CancelButtonHandlerForConfirm, this);
  }
#endif
  evas_object_show(popup_);
  evas_object_focus_set(popup_, true);

  return true;
}

bool JavaScriptModalDialogEfl::setLabelText(const char* message) {
  if (!message)
    return false;

  std::string ret;
  popupMessage_ = ("<color='#ffffff'>") + std::string(elm_entry_utf8_to_markup(message))  + ("</color>");
  base::ReplaceChars(popupMessage_, "\n", "</br>", &ret);
  popupMessage_ = ret;
  elm_object_text_set(popup_, popupMessage_.c_str());
  evas_object_event_callback_add(popup_, EVAS_CALLBACK_RESIZE, javascriptPopupResizeCallback, (void*)this);

  return true;
}

void JavaScriptModalDialogEfl::javascriptPopupResizeCallback(void *data, Evas *e, Evas_Object *obj, void *event_info) {
  if (!obj)
    return;

  int popupHeight;
  int width, height;
  ecore_x_window_size_get(ecore_x_window_root_first_get(), &width, &height);
  evas_object_geometry_get(obj, 0, 0, 0, &popupHeight);

  JavaScriptModalDialogEfl* popup = static_cast<JavaScriptModalDialogEfl*>(data);

  if (!popup)
    return;

  if (popupHeight > (height / 2)) {
    elm_object_text_set(popup->popup_, "");

    Evas_Object* layout = elm_layout_add(popup->popup_);

    base::FilePath edj_dir;
    base::FilePath javaScriptPopup_edj;
    PathService::Get(PathsEfl::EDJE_RESOURCE_DIR, &edj_dir);
    javaScriptPopup_edj = edj_dir.Append(FILE_PATH_LITERAL("JavaScriptPopup.edj"));

    elm_layout_file_set(layout, javaScriptPopup_edj.AsUTF8Unsafe().c_str(), "scroll");
    evas_object_show(layout);

    Evas_Object* scroller = elm_scroller_add(layout);
    elm_scroller_bounce_set(scroller, EINA_FALSE, EINA_TRUE);
    elm_scroller_policy_set(scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);

    evas_object_show(scroller);
    elm_object_part_content_set(layout, "scroll_container", scroller);
    elm_object_content_set(popup->popup_, layout);

    Evas_Object* label = elm_label_add(popup->popup_);
    elm_label_line_wrap_set(label, ELM_WRAP_MIXED);
    elm_object_text_set(label, popup->popupMessage_.c_str());
    evas_object_show(label);

    elm_object_content_set(scroller, label);
  }
}


Evas_Object* JavaScriptModalDialogEfl::popupAdd() {
  Evas_Object* parent = elm_object_top_widget_get(elm_object_parent_widget_get(webview_));
  if (!parent)
    return 0;

  widgetWin_ = elm_win_add(parent, "Blink JavaScript Popup", ELM_WIN_UTILITY);
  if (!widgetWin_)
    return 0;

  elm_win_alpha_set(widgetWin_, EINA_TRUE);
  ecore_x_icccm_name_class_set(elm_win_xwindow_get(widgetWin_), "APP_POPUP", "APP_POPUP");

#ifdef OS_TIZEN
  int rots[4] = {0, 90, 180, 270};
  elm_win_wm_rotation_available_rotations_set(widgetWin_, rots, 4);
#endif

  int width = 0;
  int height = 0;
  ecore_x_screen_size_get(ecore_x_default_screen_get(), &width, &height);

  evas_object_resize(widgetWin_, width, height);
  Evas_Object* conformant = elm_conformant_add(widgetWin_);
  if (!conformant)
    return 0;

  evas_object_size_hint_weight_set(conformant, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  elm_win_resize_object_add(widgetWin_, conformant);
  evas_object_show(conformant);

  Evas_Object* layout = elm_layout_add(conformant);
  if (!layout)
    return 0;

  elm_layout_theme_set(layout, "layout", "application", "default");
  evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_show(layout);

  elm_object_content_set(conformant, layout);
  elm_win_conformant_set(widgetWin_, EINA_TRUE);

  evas_object_show(widgetWin_);

  return elm_popup_add(layout);
}

void JavaScriptModalDialogEfl::OkButtonHandlerForAlert(void *data, Evas_Object *obj, void *event_info) {
  JavaScriptModalDialogEfl* dialog = (JavaScriptModalDialogEfl*)data;

  dialog->callback_.Run(true, base::string16());
  evas_object_del(elm_object_top_widget_get(obj));
  dialog->close();

  dialog->web_contents_delegate_->web_view()->SmartCallback<EWebViewCallbacks::PopupReplyWaitFinish>().call(0);
}

void JavaScriptModalDialogEfl::CancelButtonHandlerForAlert(void *data, Evas_Object *obj, void *event_info) {
  JavaScriptModalDialogEfl* dialog = (JavaScriptModalDialogEfl*)data;

  dialog->callback_.Run(false, base::string16());
  evas_object_del(elm_object_top_widget_get(obj));
  dialog->close();

  dialog->web_contents_delegate_->web_view()->SmartCallback<EWebViewCallbacks::PopupReplyWaitFinish>().call(0);
}

void JavaScriptModalDialogEfl::OkButtonHandlerForConfirm(void *data, Evas_Object *obj, void *event_info) {
  JavaScriptModalDialogEfl* dialog = (JavaScriptModalDialogEfl*)data;

  dialog->callback_.Run(true, base::string16());
  evas_object_del(elm_object_top_widget_get(obj));
  dialog->close();

  dialog->web_contents_delegate_->web_view()->SmartCallback<EWebViewCallbacks::PopupReplyWaitFinish>().call(0);
}

void JavaScriptModalDialogEfl::CancelButtonHandlerForConfirm(void *data, Evas_Object *obj, void *event_info) {
  JavaScriptModalDialogEfl* dialog = (JavaScriptModalDialogEfl*)data;

  dialog->callback_.Run(false, base::string16());
  evas_object_del(elm_object_top_widget_get(obj));
  dialog->close();

  dialog->web_contents_delegate_->web_view()->SmartCallback<EWebViewCallbacks::PopupReplyWaitFinish>().call(0);
}

void JavaScriptModalDialogEfl::OkButtonHandlerForPrompt(void *data, Evas_Object *obj, void *event_info) {
  JavaScriptModalDialogEfl* dialog = (JavaScriptModalDialogEfl*)data;

  std::string prompt_data;
  if (dialog->prompt_entry_)
    prompt_data = elm_entry_entry_get(dialog->prompt_entry_);

  dialog->callback_.Run(true, base::UTF8ToUTF16(prompt_data));
  evas_object_del(elm_object_top_widget_get(obj));
  dialog->close();

  dialog->web_contents_delegate_->web_view()->SmartCallback<EWebViewCallbacks::PopupReplyWaitFinish>().call(0);
}

void JavaScriptModalDialogEfl::CancelButtonHandlerForPrompt(void *data, Evas_Object *obj, void *event_info) {
  JavaScriptModalDialogEfl* dialog = (JavaScriptModalDialogEfl*)data;

  dialog->callback_.Run(false, base::string16());
  evas_object_del(elm_object_top_widget_get(obj));
  dialog->close();

  dialog->web_contents_delegate_->web_view()->SmartCallback<EWebViewCallbacks::PopupReplyWaitFinish>().call(0);
}

void JavaScriptModalDialogEfl::close() {
  if(widgetWin_) {
    evas_object_del(widgetWin_);
    popup_ = 0;
    widgetWin_ = 0;
  }
  prompt_entry_ = 0;
  imfContext = 0;
}

JavaScriptModalDialogEfl::~JavaScriptModalDialogEfl() {
}
