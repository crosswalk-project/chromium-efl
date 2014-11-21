// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "InputPicker.h"

#include "base/path_service.h"
#include "base/files/file_path.h"
#include "paths_efl.h"
#include "base/logging.h"
#include "base/time/time.h"
#include "content/public/browser/web_contents.h"
#include "eweb_view.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <Elementary.h>
#if defined(OS_TIZEN)
#include <vconf/vconf.h>
#endif
#if defined(OS_TIZEN_MOBILE)
#include <dlfcn.h>
#include <efl_assist.h>
extern void* EflAssistHandle;
#endif

using namespace tizen_webview;

namespace content {

static const unsigned maxDatetimeLength = 32;
static const unsigned maxDateStringLength = 10;
static const char defaultDatetimeFormat[] = "%Y/%m/%d %H:%M";

struct InputPicker::Layout {
  Layout(Evas_Object *ewkView)
    : ewk_view_(ewkView)
    , popup(0)
    , layout(0)
    , datePicker(0)
    , colorRect(0)
    , dataListEditField(0)
    , initial_r(0)
    , initial_g(0)
    , initial_b(0)
    , datetime_local(false) {
    evas_object_focus_set(ewk_view_, false);

    /* FIXME : Workaround. OSP requirement.
       OSP want to block own touch event while webkit internal picker is running. */
    evas_object_smart_callback_call(ewk_view_, "input,picker,show", 0);
  }

  ~Layout() {
    /* FIXME : Workaround. OSP requirement.
       OSP want to block own touch event while webkit internal picker is running. */
    evas_object_smart_callback_call(ewk_view_, "input,picker,hide", 0);
  }

  Evas_Object* ewk_view_;
  Evas_Object* popup;
  Evas_Object* layout;
  Evas_Object* datePicker;
  Evas_Object* colorRect;
  Evas_Object* okButton;
  Evas_Object* dataListEditField;
  int initial_r;
  int initial_g;
  int initial_b;
  bool datetime_local;
};

struct ColorPopupUserData {
  InputPicker* inputPicker;
  Evas_Object* colorRect;
  Evas_Object* color;
  Evas_Object* colorAccessObject;
};

struct Input_Date {
  int year;
  int mon;
  int day;
  int hour;
  int min;
  int sec;
};

struct Input_Date_Str {
  char year[maxDateStringLength];
  char mon[maxDateStringLength];
  char day[maxDateStringLength];
  char hour[maxDateStringLength];
  char min[maxDateStringLength];
  char sec[maxDateStringLength];
};

static char* datetimeFormat() {
#if defined(OS_TIZEN)
  char* datetimeFormat = NULL;
  char* regionFormat = NULL;
  char* language = NULL;
  char buf[256] = { 0, };
  int timeValue = 0;
  int isHour24 = 0;
  int returnValue = 0;

  language = getenv("LANGUAGE");
  setenv("LANGUAGE", "en_US", 1);

  regionFormat = vconf_get_str(VCONFKEY_REGIONFORMAT);
  if (!regionFormat)
    return 0;

  returnValue = vconf_get_int(VCONFKEY_REGIONFORMAT_TIME1224, &timeValue);
  if (returnValue < 0)
    isHour24 = 0;
  else if (timeValue == VCONFKEY_TIME_FORMAT_12 ||
      timeValue == VCONFKEY_TIME_FORMAT_24)
    isHour24 = timeValue - 1;

  if (isHour24)
    snprintf(buf, sizeof(buf), "%s_DTFMT_24HR", regionFormat);
  else
    snprintf(buf, sizeof(buf), "%s_DTFMT_12HR", regionFormat);

  free(regionFormat);

  // FIXME: Workaround fix for region format.
  int bufLength = strlen(buf);
  for (int i = 0; i < bufLength - 4; i++) {
    if (buf[i] == 'u' && buf[i + 1] == 't' && buf[i + 2] == 'f') {
      if (buf[i + 3] == '8') {
        // utf8 -> UTF-8
        for (int j = bufLength; j > i + 3; j--)
          buf[j] = buf[j - 1];
        buf[i + 3] = '-';
        buf[bufLength + 1] = '\0';
      } else if (buf[i + 3] == '-' && buf[i + 4] == '8') {
        // utf-8 -> UTF-8
      } else {
        break;
      }

      buf[i] = 'U';
      buf[i + 1] = 'T';
      buf[i + 2] = 'F';
      break;
    }
  }

  datetimeFormat = dgettext("dt_fmt", buf);

  if(!language || !strcmp(language, ""))
    unsetenv("LANGUAGE");
  else
    setenv("LANGUAGE", language, 1);

  // FIXME: Workaround fix for not supported dt_fmt.
  // Use default format if dt_fmt string is not exist.
  if (strlen(datetimeFormat) == strlen(buf) &&
      !strncmp(datetimeFormat, buf, strlen(buf))) {
    return 0;
  }

  return strdup(datetimeFormat);
#else
  return NULL;
#endif
}

InputPicker::InputPicker(EWebView& web_view_)
    : ewk_view_(web_view_.evas_object())
    , picker_layout_(0)
    , data_list_(0)
    , web_view_(web_view_) {
}

InputPicker::~InputPicker() {
  if (picker_layout_) {
    if (picker_layout_->popup)
      evas_object_del(picker_layout_->popup);
    delete picker_layout_;
  }
}

#if defined(OS_TIZEN_MOBILE)
void InputPicker::endEditingCallback(
    void* data, Evas_Object* obj, void* event_info)
{
  InputPicker* inputPicker = static_cast<InputPicker*>(data);
  if (inputPicker)
    inputPicker->removeDatetimePickerDelayed();
}
#endif

void InputPicker::showDatePicker(
    tizen_webview::Input_Type input_type, double input_date) {
  web_view_.ExecuteEditCommand("Unselect", 0);

  if (input_type == TW_INPUT_TYPE_DATE)
    showDatePopup(input_date);
  else if (input_type == TW_INPUT_TYPE_TIME)
    showTimePopup(input_date);
  else if (input_type == TW_INPUT_TYPE_DATETIME)
    showDatetimePopup(input_date, false);
  else if (input_type == TW_INPUT_TYPE_DATETIMELOCAL)
    showDatetimePopup(input_date, true);
  else if (input_type == TW_INPUT_TYPE_MONTH)
    showMonthPopup(input_date);
  else if (input_type == TW_INPUT_TYPE_WEEK)
    showWeekPopup(input_date);
}

void InputPicker::showColorPicker(int r, int g, int b, int alpha) {
    if (!picker_layout_)
  picker_layout_ = new InputPicker::Layout(ewk_view_);
  picker_layout_->popup = elm_popup_add(ewk_view_);
  elm_object_part_text_set(picker_layout_->popup, "title,text", "Select color");

  picker_layout_->initial_r = r;
  picker_layout_->initial_g = g;
  picker_layout_->initial_b = b;
#if defined(OS_TIZEN_MOBILE)
  if (EflAssistHandle) {
    void (*webkit_ea_object_event_callback_add)(
        Evas_Object *,
        Ea_Callback_Type,
        Ea_Event_Cb func,
        void *);
    webkit_ea_object_event_callback_add = (void (*)(
        Evas_Object *,
        Ea_Callback_Type,
        Ea_Event_Cb func,
        void *)) dlsym(EflAssistHandle, "ea_object_event_callback_add");
    (*webkit_ea_object_event_callback_add)(
        picker_layout_->popup, EA_CALLBACK_BACK, handleBackKeyColorPicker, this);
  }
#endif

  picker_layout_->layout = elm_layout_add(picker_layout_->popup);

  base::FilePath edj_dir;
  base::FilePath control_edj;
  PathService::Get(PathsEfl::EDJE_RESOURCE_DIR, &edj_dir);
  control_edj = edj_dir.Append(FILE_PATH_LITERAL("control.edj"));

  elm_layout_file_set(
      picker_layout_->layout,
      control_edj.AsUTF8Unsafe().c_str(),
      "color_picker");
  evas_object_size_hint_weight_set(
      picker_layout_->layout,
      EVAS_HINT_EXPAND,
      EVAS_HINT_EXPAND);
  evas_object_show(picker_layout_->layout);
  elm_object_content_set(picker_layout_->popup, picker_layout_->layout);

  picker_layout_->colorRect = evas_object_rectangle_add(
      evas_object_evas_get(picker_layout_->layout));
  evas_object_size_hint_weight_set(
      picker_layout_->colorRect,
      EVAS_HINT_EXPAND,
      EVAS_HINT_EXPAND);
  evas_object_show(picker_layout_->colorRect);

  evas_object_color_set(picker_layout_->colorRect, r, g, b, 255);
  elm_object_part_content_set(
      picker_layout_->layout,
      "elm.swallow.color_rect",
      picker_layout_->colorRect);

  static ColorPopupUserData color1Data;
  addColorRect("elm.swallow.color1", 128, 0, 0, &color1Data);

  static ColorPopupUserData color2Data;
  addColorRect("elm.swallow.color2", 255, 0, 128, &color2Data);

  static ColorPopupUserData color3Data;
  addColorRect("elm.swallow.color3", 255, 0, 0, &color3Data);

  static ColorPopupUserData color4Data;
  addColorRect("elm.swallow.color4", 255, 127, 39, &color4Data);

  static ColorPopupUserData color5Data;
  addColorRect("elm.swallow.color5", 255, 255, 0, &color5Data);

  static ColorPopupUserData color6Data;
  addColorRect("elm.swallow.color6", 0, 255, 0, &color6Data);

  static ColorPopupUserData color7Data;
  addColorRect("elm.swallow.color7", 0, 255, 255, &color7Data);

  static ColorPopupUserData color8Data;
  addColorRect("elm.swallow.color8", 0, 0, 255, &color8Data);

  static ColorPopupUserData color9Data;
  addColorRect("elm.swallow.color9", 0, 0, 128, &color9Data);

  static ColorPopupUserData color10Data;
  addColorRect("elm.swallow.color10", 64, 0, 64, &color10Data);

  static ColorPopupUserData color11Data;
  addColorRect("elm.swallow.color11", 153, 217, 234, &color11Data);

  static ColorPopupUserData color12Data;
  addColorRect("elm.swallow.color12", 128, 128, 128, &color12Data);

  static ColorPopupUserData color13Data;
  addColorRect("elm.swallow.color13", 0, 0, 0, &color13Data);

  static ColorPopupUserData color14Data;
  addColorRect("elm.swallow.color14", 255, 255, 255, &color14Data);

  // Set focus on first color rect.
  if (color1Data.colorAccessObject)
    elm_object_focus_set(color1Data.colorAccessObject, true);

  picker_layout_->okButton = elm_button_add(picker_layout_->popup);
  elm_object_style_set(picker_layout_->okButton, "popup");
  elm_object_text_set(picker_layout_->okButton, "OK");
  elm_object_part_content_set(
      picker_layout_->popup,
      "button1",
      picker_layout_->okButton);
  evas_object_smart_callback_add(
      picker_layout_->okButton,
      "clicked",
      colorPickerCallback,
      this);

  evas_object_show(picker_layout_->popup);
}

void InputPicker::hideColorPicker() {
  if (!picker_layout_)
    return;

  web_view_.web_contents().DidEndColorChooser();

  if (picker_layout_->popup) {
    evas_object_del(picker_layout_->popup);
    picker_layout_->popup = 0;
  }

  delete picker_layout_;
  picker_layout_ = 0;
}

void InputPicker::keyDownColorPickerCallback(
    void* data, Evas* evas, Evas_Object* obj, void* event_info) {
  Evas_Event_Key_Down *event = (Evas_Event_Key_Down *)event_info;
  if (!strncmp("Return", event->keyname, 6) ||
      !strncmp("space", event->keyname, 5)) {
    ColorPopupUserData* color_data = static_cast<ColorPopupUserData*>(data);
    color_data->inputPicker->selectedColorCallback(data, 0, 0, 0);
  }
}

void InputPicker::selectedColorCallback(
    void* data, Evas* evas, Evas_Object* obj, void* event_info) {
  int r = 0;
  int g = 0;
  int b = 0;
  int a = 0;

  ColorPopupUserData* color_data = static_cast<ColorPopupUserData*>(data);
  evas_object_color_get(color_data->color, &r, &g, &b, &a);
  evas_object_color_set(color_data->colorRect, r, g, b, a);
  elm_object_focus_set(color_data->colorAccessObject, true);
}

void InputPicker::addColorRect(
    const char* part, int r, int g, int b, ColorPopupUserData* color_data) {
#if defined(OS_TIZEN)
  Evas_Object* color = evas_object_rectangle_add(evas_object_evas_get(
      picker_layout_->layout));
  evas_object_size_hint_weight_set(color, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_color_set(color, r, g, b, 255);
  elm_object_part_content_set(picker_layout_->layout, part, color);
  evas_object_show(color);

  Evas_Object* accessObject = elm_access_object_register(
      color, picker_layout_->layout);
  elm_object_focus_custom_chain_append(
      picker_layout_->layout, accessObject, NULL);

  color_data->inputPicker = this;
  color_data->colorRect = picker_layout_->colorRect;
  color_data->color = color;
  color_data->colorAccessObject = accessObject;

  evas_object_event_callback_add(
      accessObject, EVAS_CALLBACK_KEY_DOWN, keyDownColorPickerCallback,
      color_data);
  evas_object_event_callback_add(
      color, EVAS_CALLBACK_MOUSE_DOWN, selectedColorCallback, color_data);
#endif
}

void InputPicker::colorPickerCallback(
    void* data,  Evas_Object* obj, void* event_info) {
  InputPicker* inputPicker = static_cast<InputPicker*>(data);

  int r(0), g(0), b(0), a(0);
  evas_object_color_get(inputPicker->picker_layout_->colorRect, &r, &g, &b, &a);

  inputPicker->web_view_.web_contents().DidChooseColorInColorChooser(
      SkColorSetARGB(a, r, g, b));
  inputPicker->hideColorPicker();
}

#if defined(OS_TIZEN_MOBILE)
void InputPicker::handleBackKeyColorPicker(
    void* data,  Evas_Object* obj, void* event_info) {
  InputPicker* inputPicker = static_cast<InputPicker*>(data);

  int r = inputPicker->picker_layout_->initial_r;
  int g = inputPicker->picker_layout_->initial_g;
  int b = inputPicker->picker_layout_->initial_b;
  int a = 255;

  inputPicker->web_view_.web_contents().DidChooseColorInColorChooser(
      SkColorSetARGB(a, r, g, b));

  inputPicker->hideColorPicker();
}
#endif

void InputPicker::showDatePopup(double input_date) {
  time_t cur_time;
  struct tm* currentTime;

  if (isnan(input_date)) {
    time(&cur_time);
    currentTime = localtime(&cur_time);
  } else {
    base::Time doubleT = base::Time::FromDoubleT(input_date/1000);
    cur_time = doubleT.ToTimeT();
    currentTime = localtime(&cur_time);
  }

  if (picker_layout_) {
    // Just update the value.
    elm_datetime_value_set(picker_layout_->datePicker, currentTime);
    evas_object_focus_set(picker_layout_->okButton, true);
    return;
  }

  picker_layout_ = new InputPicker::Layout(ewk_view_);
  createPopupLayout("Set Date", currentTime);

  picker_layout_->okButton = elm_button_add(picker_layout_->popup);
  elm_object_style_set(picker_layout_->okButton, "popup");
  elm_object_text_set(picker_layout_->okButton, "OK");
  elm_object_part_content_set(
      picker_layout_->popup, "button1", picker_layout_->okButton);
  evas_object_focus_set(picker_layout_->okButton, true);

  evas_object_smart_callback_add(
      picker_layout_->okButton, "clicked", datePopupCallback, this);
  elm_object_content_set(picker_layout_->popup, picker_layout_->layout);
  evas_object_show(picker_layout_->popup);
}

void InputPicker::showWeekPopup(double input_date) {
  time_t cur_time;
  struct tm* currentTime;

  if (isnan(input_date)) {
    time(&cur_time);
    currentTime = localtime(&cur_time);
  } else {
    base::Time doubleT = base::Time::FromDoubleT(input_date/1000);
    cur_time = doubleT.ToTimeT();
    currentTime = localtime(&cur_time);
  }

  if (picker_layout_) {
    // Just update the value.
    elm_datetime_value_set(picker_layout_->datePicker, currentTime);
    evas_object_focus_set(picker_layout_->okButton, true);
    return;
  }

  picker_layout_ = new InputPicker::Layout(ewk_view_);
  createPopupLayout("Set Week", currentTime);

  picker_layout_->okButton = elm_button_add(picker_layout_->popup);
  elm_object_style_set(picker_layout_->okButton, "popup");
  elm_object_text_set(picker_layout_->okButton, "OK");
  elm_object_part_content_set(
      picker_layout_->popup, "button1", picker_layout_->okButton);
  evas_object_focus_set(picker_layout_->okButton, true);

  evas_object_smart_callback_add(
      picker_layout_->okButton, "clicked", weekPopupCallback, this);
  elm_object_content_set(picker_layout_->popup, picker_layout_->layout);
  evas_object_show(picker_layout_->popup);
}

void InputPicker::showTimePopup(double input_date) {
  time_t cur_time;
  struct tm* currentTime;

  if (isnan(input_date)) {
    time(&cur_time);
    currentTime = localtime(&cur_time);
  } else {
    base::Time doubleT = base::Time::FromDoubleT(input_date/1000);
    cur_time = doubleT.ToTimeT();
    currentTime = localtime(&cur_time);
    currentTime->tm_hour = currentTime->tm_hour - 1;
  }

  if (picker_layout_) {
    // Just update the value.
    elm_datetime_value_set(picker_layout_->datePicker, currentTime);
    evas_object_focus_set(picker_layout_->okButton, true);
    return;
  }

  picker_layout_ = new InputPicker::Layout(ewk_view_);
  createPopupLayout("Set Time", currentTime);
  elm_object_style_set(picker_layout_->datePicker, "time_layout");

  picker_layout_->okButton = elm_button_add(picker_layout_->popup);
  elm_object_style_set(picker_layout_->okButton, "popup");
  elm_object_text_set(picker_layout_->okButton, "OK");
  elm_object_part_content_set(
      picker_layout_->popup, "button1", picker_layout_->okButton);
  evas_object_focus_set(picker_layout_->okButton, true);

  evas_object_smart_callback_add(
      picker_layout_->okButton, "clicked", timePopupCallback, this);
  elm_object_content_set(picker_layout_->popup, picker_layout_->layout);
  evas_object_show(picker_layout_->popup);
}

void InputPicker::showMonthPopup(double input_date) {
  time_t cur_time;
  struct tm* currentTime;

  if (isnan(input_date)) {
    time(&cur_time);
    currentTime = localtime(&cur_time);
  } else {
    base::Time doubleT = base::Time::FromDoubleT(input_date/1000);
    cur_time = doubleT.ToTimeT();
    currentTime = localtime(&cur_time);
  }

  if (picker_layout_) {
    // Just update the value.
    elm_datetime_value_set(picker_layout_->datePicker, currentTime);
    evas_object_focus_set(picker_layout_->okButton, true);
    return;
  }

  picker_layout_ = new InputPicker::Layout(ewk_view_);
  createPopupLayout("Set Month", currentTime);

  picker_layout_->okButton = elm_button_add(picker_layout_->popup);
  elm_object_style_set(picker_layout_->okButton, "popup");
  elm_object_text_set(picker_layout_->okButton, "OK");
  elm_object_part_content_set(
      picker_layout_->popup, "button1", picker_layout_->okButton);
  evas_object_focus_set(picker_layout_->okButton, true);

  evas_object_smart_callback_add(
      picker_layout_->okButton, "clicked", monthPopupCallback, this);
  elm_object_content_set(picker_layout_->popup, picker_layout_->layout);
  evas_object_show(picker_layout_->popup);
}

void InputPicker::showDatetimePopup(double input_date, bool datetime_local) {
  time_t cur_time;
  struct tm* currentTime;

  if (isnan(input_date)) {
    time(&cur_time);
    currentTime = localtime(&cur_time);
  } else {
    base::Time doubleT = base::Time::FromDoubleT(input_date/1000);
    cur_time = doubleT.ToTimeT();
    currentTime = localtime(&cur_time);
    currentTime->tm_hour = currentTime->tm_hour - 2;
  }

  if (picker_layout_) {
    // Just update the value.
    picker_layout_->datetime_local = datetime_local;

    elm_datetime_value_set(picker_layout_->datePicker, currentTime);
    evas_object_focus_set(picker_layout_->okButton, true);
    return;
  }

  picker_layout_ = new InputPicker::Layout(ewk_view_);
  picker_layout_->datetime_local = datetime_local;

  createPopupLayout("Set date and time", currentTime);

  picker_layout_->okButton = elm_button_add(picker_layout_->popup);
  elm_object_style_set(picker_layout_->okButton, "popup");
  elm_object_text_set(picker_layout_->okButton, "OK");
  elm_object_part_content_set(
      picker_layout_->popup, "button1", picker_layout_->okButton);
  evas_object_focus_set(picker_layout_->okButton, true);

  evas_object_smart_callback_add(
      picker_layout_->okButton, "clicked", datetimePopupCallback, this);
  elm_object_content_set(picker_layout_->popup, picker_layout_->layout);
  evas_object_show(picker_layout_->popup);

}

Eina_Bool InputPicker::removeDatetimePicker(void* data) {
  InputPicker* inputPicker = static_cast<InputPicker*>(data);
  inputPicker->web_view_.web_contents().DidCancelDialog();

  if (!inputPicker->picker_layout_)
    return ECORE_CALLBACK_CANCEL;

  inputPicker->web_view_.ExecuteEditCommand("Unselect", 0);

  inputPicker->deletePopupLayout();
  delete inputPicker->picker_layout_;
  inputPicker->picker_layout_ = 0;

  return ECORE_CALLBACK_CANCEL;
}

void InputPicker::removeDatetimePickerDelayed() {
  ecore_timer_add(0.1, removeDatetimePicker, this);
}

void InputPicker::createPopupLayout(const char* title, struct tm* currentTime) {
  picker_layout_->popup = elm_popup_add(ewk_view_);
  evas_object_size_hint_weight_set(
      picker_layout_->popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  elm_object_part_text_set(picker_layout_->popup, "title,text", title);

#if defined(OS_TIZEN_MOBILE)
  if (EflAssistHandle) {
    void (*webkit_ea_object_event_callback_add)(
        Evas_Object *, Ea_Callback_Type , Ea_Event_Cb func, void *);
    webkit_ea_object_event_callback_add = (void (*)(
        Evas_Object *,
        Ea_Callback_Type ,
        Ea_Event_Cb func,
        void *)) dlsym(EflAssistHandle, "ea_object_event_callback_add");
    (*webkit_ea_object_event_callback_add)(
        picker_layout_->popup, EA_CALLBACK_BACK, handleBackKeyDatePicker, this);
  }
#endif

  base::FilePath edj_dir;
  base::FilePath control_edj;
  PathService::Get(PathsEfl::EDJE_RESOURCE_DIR, &edj_dir);
  control_edj = edj_dir.Append(FILE_PATH_LITERAL("control.edj"));


  picker_layout_->layout = elm_layout_add(picker_layout_->popup);
  elm_layout_file_set(
      picker_layout_->layout,
      control_edj.AsUTF8Unsafe().c_str(),
      "datetime_popup");
  evas_object_size_hint_weight_set(
      picker_layout_->layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

  picker_layout_->datePicker = elm_datetime_add(picker_layout_->layout);
  elm_object_part_content_set(
      picker_layout_->layout,
      "elm.swallow.datetime",
      picker_layout_->datePicker);

  char* format = datetimeFormat();
  if (format) {
    elm_datetime_format_set(picker_layout_->datePicker, format);
    free(format);
  } else
    elm_datetime_format_set(picker_layout_->datePicker, defaultDatetimeFormat);

  elm_datetime_value_set(picker_layout_->datePicker, currentTime);

#if defined(OS_TIZEN_MOBILE)
  evas_object_smart_callback_add(
      picker_layout_->datePicker, "edit,end", endEditingCallback, 0);
#endif
}

void InputPicker::deletePopupLayout() {
  if(picker_layout_->popup){
    evas_object_del(picker_layout_->popup);
    picker_layout_->popup = 0;
  }
}

void InputPicker::datePopupCallback(
    void* data,  Evas_Object* obj, void* event_info) {
  struct tm currentTime;
  memset(&currentTime, 0, sizeof(struct tm));

  InputPicker* inputPicker = static_cast<InputPicker*>(data);

  elm_datetime_value_get(inputPicker->picker_layout_->datePicker, &currentTime);
  mktime(&currentTime);

  char dateStr[20] = { 0, };
  strftime(dateStr, 20, "%F" , &currentTime);

  inputPicker->web_view_.web_contents().DidReplaceDateTime(std::string(dateStr));
  inputPicker->web_view_.ExecuteEditCommand("Unselect", 0);

  inputPicker->deletePopupLayout();
  delete inputPicker->picker_layout_;
  inputPicker->picker_layout_ = 0;
}

void InputPicker::weekPopupCallback(
    void* data,  Evas_Object* obj, void* event_info) {
  struct tm currentTime;
  memset(&currentTime, 0, sizeof(struct tm));

  InputPicker* inputPicker = static_cast<InputPicker*>(data);

  elm_datetime_value_get(inputPicker->picker_layout_->datePicker, &currentTime);
  mktime(&currentTime);

  char dateStr[20] = { 0, };
  strftime(dateStr, 20, "%G-W%V", &currentTime);

  inputPicker->web_view_.web_contents().DidReplaceDateTime(std::string(dateStr));
  inputPicker->web_view_.ExecuteEditCommand("Unselect", 0);

  inputPicker->deletePopupLayout();
  delete inputPicker->picker_layout_;
  inputPicker->picker_layout_ = 0;
}

void InputPicker::timePopupCallback(
    void* data,  Evas_Object* obj, void* event_info) {
  struct tm currentTime;
  memset(&currentTime, 0, sizeof(struct tm));

  InputPicker* inputPicker = static_cast<InputPicker*>(data);

  elm_datetime_value_get(inputPicker->picker_layout_->datePicker, &currentTime);
  mktime(&currentTime);

  char dateStr[20] = { 0, };
  strftime(dateStr, 20, "%R", &currentTime);

  inputPicker->web_view_.web_contents().DidReplaceDateTime(std::string(dateStr));
  inputPicker->web_view_.ExecuteEditCommand("Unselect", 0);

  inputPicker->deletePopupLayout();
  delete inputPicker->picker_layout_;
  inputPicker->picker_layout_ = 0;
}

void InputPicker::monthPopupCallback(
    void* data,  Evas_Object* obj, void* event_info) {
  struct tm currentTime;
  memset(&currentTime, 0, sizeof(struct tm));

  InputPicker* inputPicker = static_cast<InputPicker*>(data);
  mktime(&currentTime);

  elm_datetime_value_get(inputPicker->picker_layout_->datePicker, &currentTime);

  char dateStr[20] = { 0, };
  strftime(dateStr, 20, "%Y-%m", &currentTime);

  inputPicker->web_view_.web_contents().DidReplaceDateTime(std::string(dateStr));
  inputPicker->web_view_.ExecuteEditCommand("Unselect", 0);

  inputPicker->deletePopupLayout();
  delete inputPicker->picker_layout_;
  inputPicker->picker_layout_ = 0;
}

void InputPicker::datetimePopupCallback(
    void* data,  Evas_Object* obj, void* event_info) {
  struct tm currentTime;
  memset(&currentTime, 0, sizeof(struct tm));

  InputPicker* inputPicker = static_cast<InputPicker*>(data);

  elm_datetime_value_get(inputPicker->picker_layout_->datePicker, &currentTime);
  mktime(&currentTime);

  char dateStr[50] = { 0, };
  if (inputPicker->picker_layout_->datetime_local)
    strftime(dateStr, 50, "%FT%R", &currentTime);
  else
    strftime(dateStr, 50, "%FT%RZ", &currentTime);

  inputPicker->web_view_.web_contents().DidReplaceDateTime(std::string(dateStr));
  inputPicker->web_view_.ExecuteEditCommand("Unselect", 0);

  inputPicker->deletePopupLayout();
  delete inputPicker->picker_layout_;
  inputPicker->picker_layout_ = 0;
}

#if defined(OS_TIZEN_MOBILE)
void InputPicker::handleBackKeyDatePicker(
    void* data, Evas_Object* obj, void* event_info) {
  removeDatetimePicker(data);
}
#endif

} // namespace content
