// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifdef TIZEN_AUTOFILL_SUPPORT

#include "autofill_popup_view_efl.h"
#include "base/path_service.h"
#include "base/files/file_path.h"
#include "base/strings/utf_string_conversions.h"
#include "eweb_view.h"
#include "paths_efl.h"

#define AUTOFILL_POPUP_LABEL_COUNT  6 // Autofill component send 6 at max
#define AUTOFILL_POPUP_LABEL_LEN    100

namespace autofill {
namespace {
  // const values taken from webkit2-efl
  static int bg_color_red = 200, bg_color_green = 200, bg_color_blue = 200;
  static int border_size = 3;
  static char labels_[AUTOFILL_POPUP_LABEL_COUNT][AUTOFILL_POPUP_LABEL_LEN];
} // namespace

enum AutofillSavePassword {
  AUTOFILL_SAVE_PASS_NEVER  = 0,
  AUTOFILL_SAVE_PASS_YES    = 1,
  AUTOFILL_SAVE_PASS_NOTNOW = 2,
};

AutofillPopupViewEfl::AutofillPopupViewEfl(EWebView* view)
  : webview_(view)
  , autofill_popup_(NULL)
  , autofill_list_(NULL)
  , password_popup_(NULL)
  , form_manager_(NULL) {
  Evas_Object* widgetWin_ = elm_object_top_widget_get(elm_object_parent_widget_get(view->evas_object()));
  if (!widgetWin_) {
    widgetWin_ = view->evas_object();
  }
  autofill_popup_ = elm_layout_add(widgetWin_);
  if (!autofill_popup_) {
    return;
  }
  base::FilePath edj_dir;
  base::FilePath autofill_edj;
  PathService::Get(PathsEfl::EDJE_RESOURCE_DIR, &edj_dir);
  autofill_edj = edj_dir.Append(FILE_PATH_LITERAL("AutofillPopup.edj"));
  elm_layout_file_set(autofill_popup_, autofill_edj.AsUTF8Unsafe().c_str(), "formdata_list");
  autofill_list_ = elm_genlist_add(autofill_popup_);
}

AutofillPopupViewEfl::~AutofillPopupViewEfl()
{
  if (autofill_popup_) {
    evas_object_del(autofill_popup_);
  }
  if (password_popup_) {
    evas_object_del(password_popup_);
  }
}

void AutofillPopupViewEfl::Show()
{
  if (autofill_popup_) {
    evas_object_show(autofill_popup_);
  }
  if (delegate_) {
    delegate_->OnPopupShown();
  }
}

void AutofillPopupViewEfl::Hide()
{
  if (autofill_popup_) {
    evas_object_hide(autofill_popup_);
  }
  if (delegate_) {
    delegate_->OnPopupHidden();
  }
}

void AutofillPopupViewEfl::ShowSavePasswordPopup(PasswordFormManager * form_to_save)
{
  if (password_popup_) {
    evas_object_del(password_popup_);
    password_popup_ = NULL;
  }
  form_manager_ = form_to_save;
  password_popup_ = elm_popup_add(webview_->evas_object());
  elm_popup_allow_events_set(password_popup_, EINA_TRUE);
  elm_popup_content_text_wrap_type_set(password_popup_, ELM_WRAP_CHAR);
  elm_object_part_text_set(password_popup_, "title,text", "Save Your Password?");
  evas_object_show(password_popup_);

  Evas_Object *btn_never = elm_button_add(password_popup_);
  elm_object_text_set(btn_never, "Never");
  elm_object_part_content_set(password_popup_, "button1", btn_never);
  evas_object_smart_callback_add(btn_never, "clicked", savePasswordNeverCb, (void*)this);

  Evas_Object *btn_yes = elm_button_add(password_popup_);
  elm_object_text_set(btn_yes, "Yes");
  elm_object_part_content_set(password_popup_, "button2", btn_yes);
  evas_object_smart_callback_add(btn_yes, "clicked", savePasswordYesCb, (void*)this);

  Evas_Object *btn_not_now = elm_button_add(password_popup_);
  elm_object_text_set(btn_not_now, "Not Now");
  elm_object_part_content_set(password_popup_, "button3", btn_not_now);
  evas_object_smart_callback_add(btn_not_now, "clicked", savePasswordNotNowCb, (void*)this);
}
           
void AutofillPopupViewEfl::UpdateFormDataPopup(const gfx::RectF& bounds)
{
  Elm_Genlist_Item_Class* list_Items = NULL;
  double scale_factor = 1.0;
  if (!autofill_list_) {
    return;
  }
  Evas_Object* border_up = elm_bg_add(autofill_popup_);
  Evas_Object* border_down = elm_bg_add(autofill_popup_);
  Evas_Object* border_left = elm_bg_add(autofill_popup_);
  Evas_Object* border_right = elm_bg_add(autofill_popup_);

  elm_genlist_clear(autofill_list_);
  list_Items = elm_genlist_item_class_new();
  list_Items->item_style = "default";
  list_Items->func.text_get = getItemLabel;
  list_Items->func.content_get = NULL;
  list_Items->func.state_get = NULL;
  list_Items->func.del = NULL;
  for (size_t i = 0; i < values_.size(); ++i) {
    elm_genlist_item_append(autofill_list_, list_Items, (void*)(long)i, NULL, ELM_GENLIST_ITEM_NONE, itemSelectCb, (void*)this);
  }
#if defined(OS_TIZEN_MOBILE)
  scale_factor = (double)gfx::Screen::GetNativeScreen()->GetPrimaryDisplay().device_scale_factor();
#elif defined(OS_TIZEN_TV)
  scale_factor = webview_->GetScale();
#endif
#ifdef OS_TIZEN
  elm_object_scale_set(autofill_list_, webview_->GetScale() / 2);
#else
  elm_object_scale_set(autofill_list_, scale_factor);
#endif
  evas_object_show(autofill_list_);
  int list_height = bounds.height() * scale_factor;
  // Show at max 3 item for mobile device
#ifdef OS_TIZEN_MOBILE
  if (values_.size() > 3) {
    list_height = 3 * list_height;
  } else
#endif
  {
    list_height = values_.size() * list_height;
  }

  // const values are taken from webkit2-efl
  elm_bg_color_set(border_up, bg_color_red, bg_color_green, bg_color_blue);
  evas_object_size_hint_min_set(border_up, bounds.width(), border_size);
  evas_object_show(border_up);

  elm_bg_color_set(border_down, bg_color_red, bg_color_green, bg_color_blue);
  evas_object_size_hint_min_set(border_down, bounds.width(), border_size);
  evas_object_show(border_down);

  elm_bg_color_set(border_left, bg_color_red, bg_color_green, bg_color_blue);
  evas_object_size_hint_min_set(border_left, border_size, list_height);
  evas_object_show(border_left);

  elm_bg_color_set(border_right, bg_color_red, bg_color_green, bg_color_blue);
  evas_object_size_hint_min_set(border_right, border_size, list_height);
  evas_object_show(border_right);

  elm_object_part_content_set(autofill_popup_, "list_container", autofill_list_);
  elm_object_part_content_set(autofill_popup_, "border_up", border_up);
  elm_object_part_content_set(autofill_popup_, "border_down", border_down);
  elm_object_part_content_set(autofill_popup_, "border_left", border_left);
  elm_object_part_content_set(autofill_popup_, "border_right", border_right);

  evas_object_size_hint_min_set(autofill_popup_, bounds.width(), list_height);
  evas_object_move(autofill_popup_, bounds.x() * scale_factor, (bounds.y() + bounds.height()) * scale_factor);
  evas_object_resize(autofill_popup_, bounds.width() * scale_factor, list_height);
  evas_object_propagate_events_set(autofill_popup_, false);
}

void AutofillPopupViewEfl::InitFormData(
          const std::vector<base::string16>& values,
          const std::vector<base::string16>& labels,
          const std::vector<base::string16>& icons,
          const std::vector<int>& identifiers,
          base::WeakPtr<AutofillPopupDelegate> delegate){
  values_ = values;
  for (size_t i = 0; i < values_.size(); ++i) {
    std::string value = UTF16ToUTF8(values_[i]);
    std::string label = UTF16ToUTF8(labels[i]);
    if(!label.empty())
      snprintf(labels_[i], sizeof(labels_[i]), "%s (%s)", value.c_str(),
          label.c_str());
    else
      snprintf(labels_[i], sizeof(labels_[i]), "%s", value.c_str());
  }
  icons_ = icons;
  identifiers_ = identifiers;
  delegate_ = delegate;
  selected_line_ = -1;
}

void AutofillPopupViewEfl::AcceptSuggestion(size_t index)
{
  if (delegate_) {
    if (index < values_.size()) {
      delegate_->DidAcceptSuggestion(values_[index], identifiers_[index]);
    }
  }
}

void AutofillPopupViewEfl::AcceptPasswordSuggestion(int option)
{
  if (password_popup_) {
    evas_object_del(password_popup_);
    password_popup_ = NULL;
  }
  if (!form_manager_) {
    return;
  }
  switch (static_cast<AutofillSavePassword>(option)) {
    case AUTOFILL_SAVE_PASS_NEVER: {
      form_manager_->PermanentlyBlacklist();
      break;
    }
    case AUTOFILL_SAVE_PASS_YES: {
      form_manager_->Save();
      break;
    }
    case AUTOFILL_SAVE_PASS_NOTNOW:
    default: {
      break;
    }
  }
}

void AutofillPopupViewEfl::SetSelectedLine(size_t selected_line)
{
  if (selected_line_ == selected_line) {
    return;
  }
  selected_line_ = selected_line;
  if(delegate_) {
    if (selected_line_ < values_.size()) {
      delegate_->DidSelectSuggestion(identifiers_[selected_line_]);
    }
    else {
      delegate_->ClearPreviewedForm();
    }
  }
}

// Static
char* AutofillPopupViewEfl::getItemLabel(void* data, Evas_Object* obj, const char* part)
{
  size_t index = (size_t)data;
  return strdup(labels_[index]);
}

void AutofillPopupViewEfl::itemSelectCb(void* data, Evas_Object* obj, void* event_info)
{
  size_t index = (size_t)elm_object_item_data_get(static_cast<Elm_Object_Item*>(event_info));
  AutofillPopupViewEfl* autofill_popup = static_cast<AutofillPopupViewEfl*>(data);
  if (autofill_popup) {
    autofill_popup->AcceptSuggestion(index);
  }
}

void AutofillPopupViewEfl::savePasswordNeverCb(void *data, Evas_Object *obj, void *event_info)
{
  AutofillPopupViewEfl * autofill_popup = static_cast<AutofillPopupViewEfl*>(data);
  if (autofill_popup) {
    autofill_popup->AcceptPasswordSuggestion(AUTOFILL_SAVE_PASS_NEVER);
  }
}

void AutofillPopupViewEfl::savePasswordYesCb(void *data, Evas_Object *obj, void *event_info)
{
  AutofillPopupViewEfl * autofill_popup = static_cast<AutofillPopupViewEfl*>(data);
  if (autofill_popup) {
    autofill_popup->AcceptPasswordSuggestion(AUTOFILL_SAVE_PASS_YES);
  }
}

void AutofillPopupViewEfl::savePasswordNotNowCb(void *data, Evas_Object *obj, void *event_info)
{
  AutofillPopupViewEfl * autofill_popup = static_cast<AutofillPopupViewEfl*>(data);
  if (autofill_popup) {
    autofill_popup->AcceptPasswordSuggestion(AUTOFILL_SAVE_PASS_NOTNOW);
  }
}

} // namespace autofill

#endif // TIZEN_AUTOFILL_SUPPORT
