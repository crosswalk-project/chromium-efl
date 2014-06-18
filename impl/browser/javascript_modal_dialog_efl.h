#ifndef JAVA_SCRIPT_MODAL_DIALOG_EFL_H_
#define JAVA_SCRIPT_MODAL_DIALOG_EFL_H_

#include "content/public/browser/javascript_dialog_manager.h"
#include "content/public/common/javascript_message_type.h"
#include <string>

#include "base/strings/string16.h"
#include "url/gurl.h"

#include <Evas.h>
#include <Elementary.h>
#include <Ecore_X.h>

namespace content {
class WebContentsDelegateEfl;
}
class JavaScriptModalDialogEfl {

 public:
  JavaScriptModalDialogEfl(content::WebContents* web_contents,
                           const GURL& origin_url,
                           const std::string& accept_lang,
                           content::JavaScriptMessageType javascript_message_type,
                           const base::string16& message_text,
                           const base::string16& default_prompt_text,
                           const content::JavaScriptDialogManager::DialogClosedCallback& callback);
  virtual ~JavaScriptModalDialogEfl();
  static void OkButtonHandlerForAlert(void *data, Evas_Object *obj, void *event_info);
  static void CancelButtonHandlerForAlert(void *data, Evas_Object *obj, void *event_info);
  static void OkButtonHandlerForConfirm(void *data, Evas_Object *obj, void *event_info);
  static void CancelButtonHandlerForConfirm(void *data, Evas_Object *obj, void *event_info);
  static void OkButtonHandlerForPrompt(void *data, Evas_Object *obj, void *event_info);
  static void CancelButtonHandlerForPrompt(void *data, Evas_Object *obj, void *event_info);
  static void javascriptPopupResizeCallback(void *data, Evas *e, Evas_Object *obj, void *event_info);
  static JavaScriptModalDialogEfl* CreateDialog(content::WebContents* web_contents,
                           const GURL& origin_url,
                           const std::string& accept_lang,
                           content::JavaScriptMessageType javascript_message_type,
                           const base::string16& message_text,
                           const base::string16& default_prompt_text,
                           const content::JavaScriptDialogManager::DialogClosedCallback& callback);

  bool ShowJavaScriptDialog();
  Evas_Object* popupAdd();
  bool setLabelText(const char* message);
  void close();
  Evas_Object* popup_;

 private:
  content::JavaScriptDialogManager::DialogClosedCallback callback_;
  GURL origin_url_;
  std::string accept_lang_;
  content::JavaScriptMessageType javascript_message_type_;
  base::string16 message_text_;
  base::string16 default_prompt_text_;
  content::WebContentsDelegateEfl* web_contents_delegate_;
  Evas_Object* window_;
  Evas_Object* prompt_entry_;
  Evas_Object* ok_button_;
  Evas_Object* cancel_button_;
  Evas_Object* webview_;
  Evas_Object* widgetWin_;
  std::string popupMessage_;
  Ecore_IMF_Context* imfContext;
};

#endif /* JAVA_SCRIPT_MODAL_DIALOG_EFL_H_ */
