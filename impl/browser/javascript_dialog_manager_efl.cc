#include "web_contents_delegate_efl.h"
#include "browser/javascript_dialog_manager_efl.h"
#include "browser/javascript_modal_dialog_efl.h"

JavaScriptModalCallbacksData::JavaScriptModalCallbacksData(content::JavaScriptMessageType javascript_message_type,
                                                           void* user_data)
  : alert_callback_(NULL), user_data_(user_data), javascript_message_type_(javascript_message_type)
{
}

bool JavaScriptModalCallbacksData::Run(Evas_Object* obj, const char* message_text, const char* default_text)
{
  switch (javascript_message_type_) {
    case content::JAVASCRIPT_MESSAGE_TYPE_ALERT: {
      if (alert_callback_)
          return alert_callback_(obj, message_text, user_data_);
      break;
    }
    case content::JAVASCRIPT_MESSAGE_TYPE_CONFIRM: {
      if (confirm_callback_)
          return confirm_callback_(obj, message_text, user_data_);
      break;
    }
    case content::JAVASCRIPT_MESSAGE_TYPE_PROMPT: {
      if (prompt_callback_)
          return prompt_callback_(obj, message_text, default_text, user_data_);
      break;
    }
    case content::JAVASCRIPT_MESSAGE_TYPE_NAVIGATION_PROMPT: {
      if (prompt_callback_)
        return prompt_callback_(obj, message_text, default_text, user_data_);
      break;
    }
  }
  return false;
}

JavaScriptModalCallbacksData* JavaScriptModalCallbacksData::CreateWithAlertDialogData(
    Ewk_View_JavaScript_Alert_Callback callback,
    void* user_data)
{
  JavaScriptModalCallbacksData* callback_data = new JavaScriptModalCallbacksData(
      content::JAVASCRIPT_MESSAGE_TYPE_ALERT, user_data);
  callback_data->alert_callback_ = callback;
  return callback_data;
}

JavaScriptModalCallbacksData* JavaScriptModalCallbacksData::CreateWithConfirmDialogData(
    Ewk_View_JavaScript_Confirm_Callback callback,
    void* user_data)
{
  JavaScriptModalCallbacksData* callback_data = new JavaScriptModalCallbacksData(
      content::JAVASCRIPT_MESSAGE_TYPE_CONFIRM, user_data);
  callback_data->confirm_callback_ = callback;
  return callback_data;
}

JavaScriptModalCallbacksData* JavaScriptModalCallbacksData::CreateWithPromptDialogData(
    Ewk_View_JavaScript_Prompt_Callback callback,
    void* user_data)
{
  JavaScriptModalCallbacksData* callback_data = new JavaScriptModalCallbacksData(
      content::JAVASCRIPT_MESSAGE_TYPE_PROMPT, user_data);
  callback_data->prompt_callback_ = callback;
  return callback_data;
}

JavaScriptDialogManagerEfl::JavaScriptDialogManagerEfl()
    : dialog_(NULL) {
}

JavaScriptDialogManagerEfl::~JavaScriptDialogManagerEfl()
{
}

void JavaScriptDialogManagerEfl::RunJavaScriptDialog(content::WebContents* web_contents,
                                                     const GURL& origin_url,
                                                     const std::string& accept_lang,
                                                     content::JavaScriptMessageType javascript_message_type,
                                                     const base::string16& message_text,
                                                     const base::string16& default_prompt_text,
                                                     const DialogClosedCallback& callback,
                                                     bool* did_suppress_message) {
  DCHECK(web_contents);
  dialog_closed_callback_ = callback;
  content::WebContentsDelegateEfl* web_contents_delegate =
      static_cast<content::WebContentsDelegateEfl*>(web_contents->GetDelegate());
  web_contents_delegate->web_view()->SmartCallback<EWebViewCallbacks::PopupReplyWaitStart>().call(0);

  if (javascript_message_type == content::JAVASCRIPT_MESSAGE_TYPE_ALERT) {
    if (alert_callback_data_) {
      if (!(alert_callback_data_->Run(web_contents_delegate->web_view()->evas_object(),
                                      UTF16ToUTF8(message_text).c_str(),
                                      NULL)))
        ExecuteDialogClosedCallBack(false, std::string());
    }
  }
  if (javascript_message_type == content::JAVASCRIPT_MESSAGE_TYPE_CONFIRM) {
    if (confirm_callback_data_) {
      if (!(confirm_callback_data_->Run(web_contents_delegate->web_view()->evas_object(),
                                        UTF16ToUTF8(message_text).c_str(),
                                        NULL)))
        ExecuteDialogClosedCallBack(false, std::string());
    }
  }
  if (javascript_message_type == content::JAVASCRIPT_MESSAGE_TYPE_PROMPT) {
    if (prompt_callback_data_) {
      if (!(prompt_callback_data_->Run(web_contents_delegate->web_view()->evas_object(),
                                       UTF16ToUTF8(message_text).c_str(),
                                       UTF16ToUTF8(default_prompt_text).c_str())))
        ExecuteDialogClosedCallBack(false, std::string());
    }
  }

  dialog_ = JavaScriptModalDialogEfl::CreateDialog(web_contents, origin_url, accept_lang, javascript_message_type, message_text, default_prompt_text, callback);
}

void JavaScriptDialogManagerEfl::SetAlertCallback(Ewk_View_JavaScript_Alert_Callback callback, void* user_data)
{
  alert_callback_data_.reset(JavaScriptModalCallbacksData::CreateWithAlertDialogData(callback, user_data));
}

void JavaScriptDialogManagerEfl::SetConfirmCallback(Ewk_View_JavaScript_Confirm_Callback callback, void* user_data)
{
  confirm_callback_data_.reset(JavaScriptModalCallbacksData::CreateWithConfirmDialogData(callback, user_data));
}

void JavaScriptDialogManagerEfl::SetPromptCallback(Ewk_View_JavaScript_Prompt_Callback callback, void* user_data)
{
  prompt_callback_data_.reset(JavaScriptModalCallbacksData::CreateWithPromptDialogData(callback, user_data));
}

void JavaScriptDialogManagerEfl::ExecuteDialogClosedCallBack(bool result, const std::string prompt_data)
{
  dialog_closed_callback_.Run(result, base::UTF8ToUTF16(prompt_data));
  if(dialog_) {
    dialog_->close();
    delete dialog_;
  }
}

void JavaScriptDialogManagerEfl::RunBeforeUnloadDialog(content::WebContents* web_contents,
                                     const base::string16& message_text,
                                     bool is_reload,
                                     const DialogClosedCallback& callback) {
  RunJavaScriptDialog(web_contents, GURL(), std::string(), content::JAVASCRIPT_MESSAGE_TYPE_NAVIGATION_PROMPT, base::UTF8ToUTF16(std::string("Confirm Navigation")), message_text, callback, &is_reload);
}
