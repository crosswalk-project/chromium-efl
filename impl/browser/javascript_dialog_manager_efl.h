// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef JAVASCRIPT_DIALOG_MANAGER_EFL_H_
#define JAVASCRIPT_DIALOG_MANAGER_EFL_H_

#include <string>
#include "base/strings/string16.h"
#include "content/public/browser/javascript_dialog_manager.h"
#include "content/public/common/javascript_message_type.h"
#include "eweb_view.h"
#include "url/gurl.h"

#include "tizen_webview/public/tw_content_security_policy.h"

namespace content{
class WebContents;
}
class JavaScriptModalDialogEfl;

class JavaScriptModalCallbacksData {
 public:
  bool Run(Evas_Object* obj, const char* content_text, const char* default_text);
  static JavaScriptModalCallbacksData* CreateWithAlertDialogData(tizen_webview::View_JavaScript_Alert_Callback callback,
                                                          void* user_data);
  static JavaScriptModalCallbacksData* CreateWithConfirmDialogData(tizen_webview::View_JavaScript_Confirm_Callback callback,
                                                            void* user_data);
  static JavaScriptModalCallbacksData* CreateWithPromptDialogData(tizen_webview::View_JavaScript_Prompt_Callback callback,
                                                           void* user_data);
  ~JavaScriptModalCallbacksData() { }

 private:
  JavaScriptModalCallbacksData(content::JavaScriptMessageType javascript_message_type, void* user_data);
  union {
    tizen_webview::View_JavaScript_Alert_Callback alert_callback_;
    tizen_webview::View_JavaScript_Confirm_Callback confirm_callback_;
    tizen_webview::View_JavaScript_Prompt_Callback prompt_callback_;
  };
  void* user_data_;
  content::JavaScriptMessageType javascript_message_type_;

};

class JavaScriptDialogManagerEfl: public content::JavaScriptDialogManager {
 public:
  JavaScriptDialogManagerEfl();
  virtual ~JavaScriptDialogManagerEfl();
  virtual void RunJavaScriptDialog(
        content::WebContents* web_contents,
        const GURL& origin_url,
        const std::string& accept_lang,
        content::JavaScriptMessageType javascript_message_type,
        const base::string16& message_text,
        const base::string16& default_prompt_text,
        const DialogClosedCallback& callback,
        bool* did_suppress_message) override;

  virtual void RunBeforeUnloadDialog(content::WebContents* web_contents,
                                     const base::string16& message_text,
                                     bool is_reload,
                                     const DialogClosedCallback& callback) override;

  virtual bool HandleJavaScriptDialog(content::WebContents* web_contents,
                                      bool accept,
                                      const base::string16* prompt_override) override
  {
    NOTIMPLEMENTED();
    return false;
  }

  virtual void CancelActiveAndPendingDialogs(content::WebContents* web_contents) override
  {
    NOTIMPLEMENTED();
  }

  virtual void WebContentsDestroyed(content::WebContents* web_contents) override
  {
    NOTIMPLEMENTED();
  }

  void SetAlertCallback(tizen_webview::View_JavaScript_Alert_Callback callback, void* user_data);
  void SetConfirmCallback(tizen_webview::View_JavaScript_Confirm_Callback callback, void* user_data);
  void SetPromptCallback(tizen_webview::View_JavaScript_Prompt_Callback callback, void* user_data);
  void ExecuteDialogClosedCallBack(bool result, const std::string prompt_data);

 private:
  JavaScriptModalDialogEfl* dialog_;
  scoped_ptr<JavaScriptModalCallbacksData> alert_callback_data_;
  scoped_ptr<JavaScriptModalCallbacksData> confirm_callback_data_;
  scoped_ptr<JavaScriptModalCallbacksData> prompt_callback_data_;
  DialogClosedCallback dialog_closed_callback_;

};

#endif /* JAVASCRIPT_DIALOG_MANAGER_EFL_H_ */
