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

#include "InputPicker.h"

#include "base/path_service.h"
#include "base/files/file_path.h"
#include "paths_efl.h"
#include "base/logging.h"
#include "content/public/browser/web_contents.h"
#include "eweb_view.h"
#include <Elementary.h>
#include <stdlib.h>
#include <string.h>
#ifdef OS_TIZEN
#include <vconf/vconf.h>
#endif

#ifdef OS_TIZEN_MOBILE
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
#ifdef OS_TIZEN_MOBILE
    , initial_r(0)
    , initial_g(0)
    , initial_b(0)
#endif
    , datetimeLocal(false) {
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
#ifdef OS_TIZEN_MOBILE
  int initial_r;
  int initial_g;
  int initial_b;
#endif
  bool datetimeLocal;
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
#ifdef OS_TIZEN
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
  else if (timeValue == VCONFKEY_TIME_FORMAT_12 || timeValue == VCONFKEY_TIME_FORMAT_24)
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

  // FIXME: Workaround fix for not supported dt_fmt. Use default format if dt_fmt string is not exist.
  if (strlen(datetimeFormat) == strlen(buf) && !strncmp(datetimeFormat, buf, strlen(buf))) {
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

#ifdef OS_TIZEN_MOBILE
void InputPicker::_edit_end_cb(void* data, Evas_Object* obj, void* event_info)
{
  InputPicker* inputPicker = static_cast<InputPicker*>(data);
  if (inputPicker)
    inputPicker->removeDatetimePickerDelayed();
}
#endif

void InputPicker::show(tizen_webview::Input_Type inputType, const char* inputValue) {
  web_view_.ExecuteEditCommand("Unselect", 0);

  if (inputType == TW_INPUT_TYPE_DATE)
    ewk_date_popup(inputValue);
  else if (inputType == TW_INPUT_TYPE_TIME)
    ewk_time_popup(inputValue);
  else if (inputType == TW_INPUT_TYPE_DATETIME)
    ewk_datetime_popup(inputValue, false);
  else if (inputType == TW_INPUT_TYPE_DATETIMELOCAL)
    ewk_datetime_popup(inputValue, true);
  else if (inputType == TW_INPUT_TYPE_MONTH)
    ewk_month_popup(inputValue);
  else if (inputType == TW_INPUT_TYPE_WEEK)
    ewk_week_popup(inputValue);
}

void InputPicker::showColorPicker(int r, int g, int b, int) {
  ewk_color_popup(r, g, b);
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

// FIXME : DJKim : not implemented yet
#if 0 //ENABLE(ELM_COLORPALLETE)
static void _color_palette_changed_cb(void* data, Evas_Object* obj, void* eventInfo) {
  int r = 0;
  int g = 0;
  int b = 0;
  int a = 0;
  Elm_Object_Item *color_it = static_cast<Elm_Object_Item*>(eventInfo);
  elm_colorselector_palette_item_color_get(color_it, &r, &g, &b, &a);
  evas_object_color_set(static_cast<Evas_Object*>(data), r, g, b , a);
}
#endif

void InputPicker::colorKeyDownCallback(void* data, Evas* evas, Evas_Object* obj, void* eventInfo) {
  Evas_Event_Key_Down *event = (Evas_Event_Key_Down *)eventInfo;
  if (!strncmp("Return", event->keyname, 6) || !strncmp("space", event->keyname, 5)) {
    ColorPopupUserData* colorData = static_cast<ColorPopupUserData*>(data);
    colorData->inputPicker->_color_selected_cb(data, 0, 0, 0);
  }
}

void InputPicker::_color_selected_cb(void* data, Evas* evas, Evas_Object* obj, void* eventInfo) {
  int r = 0;
  int g = 0;
  int b = 0;
  int a = 0;

  ColorPopupUserData* colorData = static_cast<ColorPopupUserData*>(data);
  evas_object_color_get(colorData->color, &r, &g, &b, &a);
  evas_object_color_set(colorData->colorRect, r, g, b, a);
  elm_object_focus_set(colorData->colorAccessObject, true);
}

void InputPicker::addColorRect(const char* part, int r, int g, int b, ColorPopupUserData* colorData) {
#ifdef OS_TIZEN
  Evas_Object* color = evas_object_rectangle_add(evas_object_evas_get(picker_layout_->layout));
  evas_object_size_hint_weight_set(color, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_color_set(color, r, g, b, 255);
  elm_object_part_content_set(picker_layout_->layout, part, color);
  evas_object_show(color);

  Evas_Object* accessObject = elm_access_object_register(color, picker_layout_->layout);
  elm_object_focus_custom_chain_append(picker_layout_->layout, accessObject, NULL);

  colorData->inputPicker = this;
  colorData->colorRect = picker_layout_->colorRect;
  colorData->color = color;
  colorData->colorAccessObject = accessObject;

  evas_object_event_callback_add(accessObject, EVAS_CALLBACK_KEY_DOWN, colorKeyDownCallback, colorData);
  evas_object_event_callback_add(color, EVAS_CALLBACK_MOUSE_DOWN, _color_selected_cb, colorData);
#endif
}

void InputPicker::ewk_color_popup(int r, int g, int b) {
  if (picker_layout_)
    return;

  picker_layout_->popup = elm_popup_add(elm_object_parent_widget_get(ewk_view_));

  picker_layout_->popup = elm_popup_add(ewk_view_);
  elm_object_part_text_set(picker_layout_->popup, "title,text", "Select color");

#ifdef OS_TIZEN_MOBILE
  picker_layout_->initial_r = r;
  picker_layout_->initial_g = g;
  picker_layout_->initial_b = b;
  if (EflAssistHandle)
  {
    void (*webkit_ea_object_event_callback_add)(Evas_Object *, Ea_Callback_Type , Ea_Event_Cb func, void *);
    webkit_ea_object_event_callback_add = (void (*)(Evas_Object *, Ea_Callback_Type , Ea_Event_Cb func, void *))dlsym(EflAssistHandle, "ea_object_event_callback_add");
    (*webkit_ea_object_event_callback_add)(picker_layout_->popup, EA_CALLBACK_BACK, _color_back_cb, this);
  }
#endif

  picker_layout_->layout = elm_layout_add(picker_layout_->popup);

  base::FilePath edj_dir;
  base::FilePath control_edj;
  PathService::Get(PathsEfl::EDJE_RESOURCE_DIR, &edj_dir);
  control_edj = edj_dir.Append(FILE_PATH_LITERAL("control.edj"));

  elm_layout_file_set(picker_layout_->layout, control_edj.AsUTF8Unsafe().c_str(),"color_picker");
  evas_object_size_hint_weight_set(picker_layout_->layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_show(picker_layout_->layout);
  elm_object_content_set(picker_layout_->popup, picker_layout_->layout);

  picker_layout_->colorRect = evas_object_rectangle_add(evas_object_evas_get(picker_layout_->layout));
  evas_object_size_hint_weight_set(picker_layout_->colorRect, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_show(picker_layout_->colorRect);

  evas_object_color_set(picker_layout_->colorRect, r, g, b, 255);
  elm_object_part_content_set(picker_layout_->layout, "elm.swallow.color_rect", picker_layout_->colorRect);

  // FIXME : DJKim : not implemented yet
#if 0 //ENABLE(ELM_COLORPALLETE)
  Evas_Object* colorPalette = elm_colorselector_add(picker_layout_->popup);
  elm_colorselector_mode_set(colorPalette, ELM_COLORSELECTOR_PALETTE);

  elm_colorselector_palette_color_add(colorPalette, 128, 0, 0, 255);
  elm_colorselector_palette_color_add(colorPalette, 255, 0, 128, 255);
  elm_colorselector_palette_color_add(colorPalette, 255, 0, 0, 255);
  elm_colorselector_palette_color_add(colorPalette, 255, 127, 39, 255);
  elm_colorselector_palette_color_add(colorPalette, 255, 255, 0, 255);
  elm_colorselector_palette_color_add(colorPalette, 0, 255, 0, 255);
  elm_colorselector_palette_color_add(colorPalette, 0, 255, 255, 255);
  elm_colorselector_palette_color_add(colorPalette, 0, 0, 255, 255);
  elm_colorselector_palette_color_add(colorPalette, 0, 0, 128, 255);
  elm_colorselector_palette_color_add(colorPalette, 64, 0, 64, 255);
  elm_colorselector_palette_color_add(colorPalette, 0, 0, 0, 255);
  elm_colorselector_palette_color_add(colorPalette, 255, 255, 255, 255);

  evas_object_size_hint_fill_set(colorPalette, EVAS_HINT_FILL, 0);
  evas_object_size_hint_weight_set(colorPalette, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_smart_callback_add(colorPalette, "color,item,selected", _color_palette_changed_cb, picker_layout_->colorRect);
  evas_object_show(colorPalette);

  elm_object_part_content_set(picker_layout_->layout, "elm.swallow.color_palette", colorPalette);
#else
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
#endif
  picker_layout_->okButton = elm_button_add(picker_layout_->popup);
  elm_object_style_set(picker_layout_->okButton, "popup");
  elm_object_text_set(picker_layout_->okButton, "OK");
  elm_object_part_content_set(picker_layout_->popup, "button1", picker_layout_->okButton);
  evas_object_smart_callback_add(picker_layout_->okButton, "clicked", _color_popup_response_cb, this);

  evas_object_show(picker_layout_->popup);

}

void InputPicker::_color_popup_response_cb(void* data,  Evas_Object* obj, void* event_info) {
  InputPicker* inputPicker = static_cast<InputPicker*>(data);

  int r = 0;
  int g = 0;
  int b = 0;
  int a = 0;
  evas_object_color_get(inputPicker->picker_layout_->colorRect, &r, &g, &b, &a);

  inputPicker->web_view_.web_contents().DidChooseColorInColorChooser(SkColorSetARGB(a, r, g, b));
  inputPicker->hideColorPicker();
}

#ifdef OS_TIZEN_MOBILE
void InputPicker::_color_back_cb(void* data,  Evas_Object* obj, void* event_info) {
  InputPicker* inputPicker = static_cast<InputPicker*>(data);

  int r = inputPicker->picker_layout_->initial_r;
  int g = inputPicker->picker_layout_->initial_g;
  int b = inputPicker->picker_layout_->initial_b;
  int a = 255;

  inputPicker->web_view_.web_contents().DidChooseColorInColorChooser(SkColorSetARGB(a, r, g, b));

  inputPicker->hideColorPicker();
}
#endif

void InputPicker::ewk_date_popup(const char* inputValue) {
  struct tm* currentTime;
  time_t cur_time;
  time(&cur_time);
  currentTime = localtime(&cur_time);

  Input_Date_Str dateStr;
  memset(&dateStr, 0, sizeof(Input_Date_Str));

  if (inputValue && strlen(inputValue)) {
    char tmpinputValue[maxDatetimeLength] = { 0, };

    snprintf(tmpinputValue, maxDatetimeLength, "%s", inputValue);
    char* token = strtok(tmpinputValue,"-");
    if (token)
      strncpy(dateStr.year, token, maxDateStringLength);
    token = strtok(0, "-");
    if (token)
      strncpy(dateStr.mon, token, maxDateStringLength);
    token = strtok(0, "-");
    if (token)
      strncpy(dateStr.day, token, maxDateStringLength);

    if (dateStr.year)
      currentTime->tm_year = atoi(dateStr.year);
    if (dateStr.mon)
      currentTime->tm_mon = atoi(dateStr.mon);
    if (dateStr.day)
      currentTime->tm_mday = atoi(dateStr.day);

    currentTime->tm_year = currentTime->tm_year - 1900;
    currentTime->tm_mon = currentTime->tm_mon - 1;
  }

  if (picker_layout_) {
    // Just update the value.
    elm_datetime_value_set(picker_layout_->popup, currentTime);
    elm_object_focus_set(picker_layout_->popup, true);
    return;
  }

  picker_layout_ = new InputPicker::Layout(ewk_view_);

  createDatetimePicker(currentTime);
  elm_datetime_field_visible_set(picker_layout_->popup, ELM_DATETIME_HOUR, EINA_FALSE);
  elm_datetime_field_visible_set(picker_layout_->popup, ELM_DATETIME_MINUTE, EINA_FALSE);
  elm_datetime_field_visible_set(picker_layout_->popup, ELM_DATETIME_AMPM, EINA_FALSE);

  evas_object_smart_callback_add(picker_layout_->popup, "picker,value,set", _date_popup_response_cb, this);
#ifdef OS_TIZEN_MOBILE
  if (EflAssistHandle) {
    void (*webkit_ea_object_event_callback_add)(Evas_Object *, Ea_Callback_Type , Ea_Event_Cb func, void *);
    webkit_ea_object_event_callback_add = (void (*)(Evas_Object *, Ea_Callback_Type , Ea_Event_Cb func, void *))dlsym(EflAssistHandle, "ea_object_event_callback_add");
    (*webkit_ea_object_event_callback_add)(picker_layout_->popup, EA_CALLBACK_BACK, _popup_back_cb, this);
  }
#endif

  evas_object_focus_set(picker_layout_->popup, true);
  evas_object_show(picker_layout_->popup);
  elm_object_signal_emit(picker_layout_->popup, "datepicker,show", "");
}

#define isLeapYear(year) ((!(year % 4) && (year % 100)) || (!(year % 400) && (year % 1000)))
static int _calculateMonthAndDay(int year, int totalDays, int* remains) {
  // Jan.
  if (totalDays - 31 < 0) {
    *remains = totalDays;
    return 0;
  }

  totalDays = totalDays - 31;

  // Feb.
  if (isLeapYear(year)) {
    if (totalDays - 29 < 0) {
      *remains = totalDays;
      return 1;
    }
    totalDays = totalDays - 29;
  } else {
    if (totalDays - 28 < 0) {
      *remains = totalDays;
      return 1;
    }
    totalDays = totalDays - 28;
  }

  // Mar.
  if (totalDays - 31 < 0) {
    *remains = totalDays;
    return 2;
  }
  totalDays = totalDays - 31;

  // Apr.
  if (totalDays - 30 < 0) {
    *remains = totalDays;
    return 3;
  }
  totalDays = totalDays - 30;

  // May
  if (totalDays - 31 < 0) {
    *remains = totalDays;
    return 4;
  }
  totalDays = totalDays - 31;

  // Jun.
  if (totalDays - 30 < 0) {
    *remains = totalDays;
    return 5;
  }
  totalDays = totalDays - 30;

  // Jul.
  if (totalDays - 31 < 0) {
    *remains = totalDays;
    return 6;
  }
  totalDays = totalDays - 31;

  // Aug.
  if (totalDays - 31 < 0) {
    *remains = totalDays;
    return 7;
  }
  totalDays = totalDays - 31;

  // Sept.
  if (totalDays - 30 < 0) {
    *remains = totalDays;
    return 8;
  }
  totalDays = totalDays - 30;

  // Oct.
  if (totalDays - 31 < 0) {
    *remains = totalDays;
    return 9;
  }
  totalDays = totalDays - 31;

  // Nov.
  if (totalDays - 30 < 0) {
    *remains = totalDays;
    return 10;
  }
  totalDays = totalDays - 30;

  *remains = totalDays;
  return 11;
}

void InputPicker::ewk_week_popup(const char* inputValue) {
  struct tm* currentTime;
  time_t cur_time;
  time(&cur_time);
  currentTime = localtime(&cur_time);

  Input_Date_Str dateStr;
  memset(&dateStr, 0, sizeof(Input_Date_Str));

  if (inputValue && strlen(inputValue)) {
    char tmpinputValue[maxDatetimeLength] = { 0, };

    snprintf(tmpinputValue, maxDatetimeLength, "%s", inputValue);
    char* token = strtok(tmpinputValue,"-");
    if (token)
      strncpy(dateStr.year, token, maxDateStringLength);
    const char* week = strstr(inputValue, "W");
    int weekNum = 1;
    if (week)
      weekNum = atoi(week + 1);

    if (dateStr.year)
      currentTime->tm_year = atoi(dateStr.year);
    currentTime->tm_year = currentTime->tm_year - 1900;

    struct tm firtTimeOfyear;
    memset(&firtTimeOfyear, 0, sizeof(struct tm));
    firtTimeOfyear.tm_year = currentTime->tm_year;
    firtTimeOfyear.tm_mon = 0;
    firtTimeOfyear.tm_mday = 1;
    mktime(&firtTimeOfyear);

    char firstWeek[10] = {0, };
    strftime(firstWeek, 10, "%w", &firtTimeOfyear);
    int firstWeekCount = atoi(firstWeek);

    int totalDays = 1;

    totalDays = weekNum * 7 - firstWeekCount;

    int days = 0;
    int month = _calculateMonthAndDay(currentTime->tm_year, totalDays, &days);

    currentTime->tm_mon = month;
    currentTime->tm_mday = days;
  }

  if (picker_layout_) {
    // Just update the value.
    elm_datetime_value_set(picker_layout_->popup, currentTime);
    elm_object_focus_set(picker_layout_->popup, true);
    return;
  }

  picker_layout_ = new InputPicker::Layout(ewk_view_);

  createDatetimePicker(currentTime);
  elm_datetime_field_visible_set(picker_layout_->datePicker, ELM_DATETIME_HOUR, EINA_FALSE);
  elm_datetime_field_visible_set(picker_layout_->datePicker, ELM_DATETIME_MINUTE, EINA_FALSE);
  elm_datetime_field_visible_set(picker_layout_->datePicker, ELM_DATETIME_AMPM, EINA_FALSE);

  evas_object_smart_callback_add(picker_layout_->popup, "picker,value,set", _week_popup_response_cb, this);
#ifdef OS_TIZEN_MOBILE
  if (EflAssistHandle) {
    void (*webkit_ea_object_event_callback_add)(Evas_Object *, Ea_Callback_Type , Ea_Event_Cb func, void *);
    webkit_ea_object_event_callback_add = (void (*)(Evas_Object *, Ea_Callback_Type , Ea_Event_Cb func, void *))dlsym(EflAssistHandle, "ea_object_event_callback_add");
    (*webkit_ea_object_event_callback_add)(picker_layout_->popup, EA_CALLBACK_BACK, _popup_back_cb, this);
  }
#endif
  evas_object_focus_set(picker_layout_->popup, true);
  evas_object_show(picker_layout_->popup);
  elm_object_signal_emit(picker_layout_->popup, "datepicker,show", "");
}

void InputPicker::ewk_time_popup(const char* inputValue) {
  struct tm* currentTime;
  time_t  cur_time;
  time(&cur_time);
  currentTime = localtime(&cur_time);

  Input_Date_Str dateStr;
  memset(&dateStr, 0, sizeof(Input_Date_Str));

  if (inputValue && strlen(inputValue)) {
    char tmpinputValue[maxDatetimeLength] = { 0, };

    snprintf(tmpinputValue, maxDatetimeLength, "%s", inputValue);
    char* token = strtok(tmpinputValue,":");
    if (token)
      strncpy(dateStr.hour, token, maxDateStringLength);
    token = strtok(0, ":");
    if (token)
      strncpy(dateStr.min, token, maxDateStringLength);

    if (dateStr.hour)
      currentTime->tm_hour = atoi(dateStr.hour);
    if (dateStr.min)
      currentTime->tm_min = atoi(dateStr.min);
  }

  if (picker_layout_) {
    // Just update the value.
    elm_datetime_value_set(picker_layout_->popup, currentTime);
    elm_object_focus_set(picker_layout_->popup, true);
    return;
  }

  picker_layout_ = new InputPicker::Layout(ewk_view_);

  createDatetimePicker(currentTime);
  elm_datetime_field_visible_set(picker_layout_->popup, ELM_DATETIME_YEAR, EINA_FALSE);
  elm_datetime_field_visible_set(picker_layout_->popup, ELM_DATETIME_MONTH, EINA_FALSE);
  elm_datetime_field_visible_set(picker_layout_->popup, ELM_DATETIME_DATE, EINA_FALSE);

  evas_object_smart_callback_add(picker_layout_->popup, "picker,value,set", _time_popup_response_cb, this);
#ifdef OS_TIZEN_MOBILE
  if (EflAssistHandle) {
    void (*webkit_ea_object_event_callback_add)(Evas_Object *, Ea_Callback_Type , Ea_Event_Cb func, void *);
    webkit_ea_object_event_callback_add = (void (*)(Evas_Object *, Ea_Callback_Type , Ea_Event_Cb func, void *))dlsym(EflAssistHandle, "ea_object_event_callback_add");
    (*webkit_ea_object_event_callback_add)(picker_layout_->popup, EA_CALLBACK_BACK, _popup_back_cb, this);
  }
#endif
  evas_object_focus_set(picker_layout_->popup, true);
  evas_object_show(picker_layout_->popup);
  elm_object_signal_emit(picker_layout_->popup, "timepicker,show", "");
}

void InputPicker::ewk_month_popup(const char* inputValue) {
  struct tm* currentTime;
  time_t  cur_time;
  time(&cur_time);
  currentTime = localtime(&cur_time);

  Input_Date_Str dateStr;
  memset(&dateStr, 0, sizeof(Input_Date_Str));

  if (inputValue && strlen(inputValue)) {
    char tmpInputValue[maxDatetimeLength] = { 0, };

    snprintf(tmpInputValue, maxDatetimeLength, "%s", inputValue);
    char* token = strtok(tmpInputValue,"-");
    if (token)
      strncpy(dateStr.year, token, maxDateStringLength);
    token = strtok(0, "-");
    if (token)
      strncpy(dateStr.mon, token, maxDateStringLength);

    if (dateStr.year)
      currentTime->tm_year = atoi(dateStr.year);
    if (dateStr.mon)
      currentTime->tm_mon = atoi(dateStr.mon);

    currentTime->tm_year = currentTime->tm_year - 1900;
    currentTime->tm_mon = currentTime->tm_mon - 1;
  }

  if (picker_layout_) {
    // Just update the value.
    elm_datetime_value_set(picker_layout_->popup, currentTime);
    elm_object_focus_set(picker_layout_->popup, true);
    return;
  }

  picker_layout_ = new InputPicker::Layout(ewk_view_);

  createDatetimePicker(currentTime);
  elm_datetime_field_visible_set(picker_layout_->datePicker, ELM_DATETIME_DATE, EINA_FALSE);
  elm_datetime_field_visible_set(picker_layout_->datePicker, ELM_DATETIME_HOUR, EINA_FALSE);
  elm_datetime_field_visible_set(picker_layout_->datePicker, ELM_DATETIME_MINUTE, EINA_FALSE);
  elm_datetime_field_visible_set(picker_layout_->datePicker, ELM_DATETIME_AMPM, EINA_FALSE);
  elm_datetime_format_set(picker_layout_->datePicker, "%Y/%m");

  evas_object_smart_callback_add(picker_layout_->popup, "picker,value,set", _month_popup_response_cb, this);
#ifdef OS_TIZEN_MOBILE
  if (EflAssistHandle) {
    LOG(ERROR) << "[DJKim] ++++++++++++++++++++++++++++++++++++ EA_CALLBACK_BACK" ;
    void (*webkit_ea_object_event_callback_add)(Evas_Object *, Ea_Callback_Type , Ea_Event_Cb func, void *);
    webkit_ea_object_event_callback_add = (void (*)(Evas_Object *, Ea_Callback_Type , Ea_Event_Cb func, void *))dlsym(EflAssistHandle, "ea_object_event_callback_add");
    (*webkit_ea_object_event_callback_add)(picker_layout_->popup, EA_CALLBACK_BACK, _popup_back_cb, this);
  }
#endif
  evas_object_focus_set(picker_layout_->popup, true);
  evas_object_show(picker_layout_->popup);
  elm_object_signal_emit(picker_layout_->popup, "datepicker,show", "");
}

void InputPicker::ewk_datetime_popup(const char* inputValue, bool local) {
  struct tm* currentTime;
  time_t  cur_time;
  time(&cur_time);
  currentTime = localtime(&cur_time);

  Input_Date_Str dateStr;
  memset(&dateStr, 0, sizeof(Input_Date_Str));

  if (inputValue && strlen(inputValue)) {
    char tmpInputValue[maxDatetimeLength] = { 0, };

    snprintf(tmpInputValue, maxDatetimeLength, "%s", inputValue);
    char* token = strtok(tmpInputValue,"-");
    if (token)
      strncpy(dateStr.year, token, maxDateStringLength);
    token = strtok(0, "-");
    if (token)
      strncpy(dateStr.mon, token, maxDateStringLength);
    token = strtok(0, "T");
    if (token)
      strncpy(dateStr.day, token, maxDateStringLength);
    token = strtok(0, ":");
    if (token)
      strncpy(dateStr.hour, token, maxDateStringLength);

    if (local) {
      token = strtok(0, "Z");
      if (token)
        strncpy(dateStr.min, token, maxDateStringLength);
    } else {
      token = strtok(0, ":");
      if (token)
        strncpy(dateStr.min, token, maxDateStringLength);
    }

    if (dateStr.year)
      currentTime->tm_year = atoi(dateStr.year);
    if (dateStr.mon)
      currentTime->tm_mon = atoi(dateStr.mon);
    if (dateStr.day)
      currentTime->tm_mday = atoi(dateStr.day);
    if (dateStr.hour)
      currentTime->tm_hour = atoi(dateStr.hour);
    if (dateStr.min)
      currentTime->tm_min = atoi(dateStr.min);

    currentTime->tm_year = currentTime->tm_year - 1900;
    currentTime->tm_mon = currentTime->tm_mon - 1;
  }

  if (picker_layout_) {
    // Just update the value.
    picker_layout_->datetimeLocal = local;

    elm_datetime_value_set(picker_layout_->datePicker, currentTime);
    evas_object_focus_set(picker_layout_->okButton, true);
    return;
  }

  picker_layout_ = new InputPicker::Layout(ewk_view_);

  picker_layout_->datetimeLocal = local;

  // FIXME : DJKim : not implemented yet
  //String popupTitle = popupTitleSetDateAndTime();
  createDatetimePopup("Set date and time", currentTime);

  picker_layout_->okButton = elm_button_add(picker_layout_->popup);
  elm_object_style_set(picker_layout_->okButton, "popup");
  elm_object_text_set(picker_layout_->okButton, "OK");
  elm_object_part_content_set(picker_layout_->popup, "button1", picker_layout_->okButton);
  evas_object_focus_set(picker_layout_->okButton, true);

  evas_object_smart_callback_add(picker_layout_->okButton, "clicked", _datetime_popup_response_cb, this);
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

void InputPicker::createDatetimePicker(struct tm* currentTime) {
  picker_layout_->popup = elm_datetime_add(elm_object_parent_widget_get(ewk_view_));
  elm_object_style_set(picker_layout_->popup, "pickerstyle");

  char* format = datetimeFormat();
  if (format) {
    elm_datetime_format_set(picker_layout_->popup, format);
    free(format);
  } else
    elm_datetime_format_set(picker_layout_->popup, defaultDatetimeFormat);

  elm_datetime_value_set(picker_layout_->popup, currentTime);

#ifdef OS_TIZEN_MOBILE
  evas_object_smart_callback_add(picker_layout_->popup, "edit,end", _edit_end_cb, this);
#endif
}

void InputPicker::createDatetimePopup(const char* title, struct tm* currentTime) {
  picker_layout_->popup = elm_popup_add(elm_object_parent_widget_get(ewk_view_));
  evas_object_size_hint_weight_set(picker_layout_->popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  elm_object_part_text_set(picker_layout_->popup, "title,text", title);

#ifdef OS_TIZEN_MOBILE
  if (EflAssistHandle) {
    void (*webkit_ea_object_event_callback_add)(Evas_Object *, Ea_Callback_Type , Ea_Event_Cb func, void *);
    webkit_ea_object_event_callback_add = (void (*)(Evas_Object *, Ea_Callback_Type , Ea_Event_Cb func, void *))dlsym(EflAssistHandle, "ea_object_event_callback_add");
    (*webkit_ea_object_event_callback_add)(picker_layout_->popup, EA_CALLBACK_BACK, _popup_back_cb, this);
  }
#endif

  base::FilePath edj_dir;
  base::FilePath control_edj;
  PathService::Get(PathsEfl::EDJE_RESOURCE_DIR, &edj_dir);
  control_edj = edj_dir.Append(FILE_PATH_LITERAL("control.edj"));


  picker_layout_->layout = elm_layout_add(picker_layout_->popup);
  elm_layout_file_set(picker_layout_->layout, control_edj.AsUTF8Unsafe().c_str(),"datetime_popup");
  evas_object_size_hint_weight_set(picker_layout_->layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

  picker_layout_->datePicker = elm_datetime_add(picker_layout_->layout);
  elm_object_part_content_set(picker_layout_->layout , "elm.swallow.datetime", picker_layout_->datePicker);

  char* format = datetimeFormat();
  if (format) {
    elm_datetime_format_set(picker_layout_->datePicker, format);
    free(format);
  } else
    elm_datetime_format_set(picker_layout_->datePicker, defaultDatetimeFormat);

  elm_datetime_value_set(picker_layout_->datePicker, currentTime);

#ifdef OS_TIZEN_MOBILE
  evas_object_smart_callback_add(picker_layout_->datePicker, "edit,end", _edit_end_cb, 0);
#endif
}

void InputPicker::deletePopupLayout() {
  if(picker_layout_->popup){
    evas_object_del(picker_layout_->popup);
    picker_layout_->popup = 0;
  }
}

void InputPicker::_date_popup_response_cb(void* data,  Evas_Object* obj, void* event_info) {
  struct tm currentTime;
  memset(&currentTime, 0, sizeof(struct tm));

  InputPicker* inputPicker = static_cast<InputPicker*>(data);

  elm_datetime_value_get(inputPicker->picker_layout_->popup, &currentTime);
  mktime(&currentTime);

  char dateStr[20] = { 0, };
  strftime(dateStr, 20, "%F" , &currentTime);

  inputPicker->web_view_.web_contents().DidReplaceDateTime(std::string(dateStr));
}

void InputPicker::_week_popup_response_cb(void* data,  Evas_Object* obj, void* event_info) {
  struct tm currentTime;
  memset(&currentTime, 0, sizeof(struct tm));

  InputPicker* inputPicker = static_cast<InputPicker*>(data);

  elm_datetime_value_get(inputPicker->picker_layout_->popup, &currentTime);
  mktime(&currentTime);

  char dateStr[20] = { 0, };
  strftime(dateStr, 20, "%G-W%V", &currentTime);

  inputPicker->web_view_.web_contents().DidReplaceDateTime(std::string(dateStr));
}

void InputPicker::_time_popup_response_cb(void* data,  Evas_Object* obj, void* event_info) {
  struct tm currentTime;
  memset(&currentTime, 0, sizeof(struct tm));

  InputPicker* inputPicker = static_cast<InputPicker*>(data);

  elm_datetime_value_get(inputPicker->picker_layout_->popup, &currentTime);
  mktime(&currentTime);

  char dateStr[20] = { 0, };
  strftime(dateStr, 20, "%R", &currentTime);

  inputPicker->web_view_.web_contents().DidReplaceDateTime(std::string(dateStr));
}

void InputPicker::_month_popup_response_cb(void* data,  Evas_Object* obj, void* event_info) {
  struct tm currentTime;
  memset(&currentTime, 0, sizeof(struct tm));

  InputPicker* inputPicker = static_cast<InputPicker*>(data);
  mktime(&currentTime);

  elm_datetime_value_get(inputPicker->picker_layout_->popup, &currentTime);

  char dateStr[20] = { 0, };
  strftime(dateStr, 20, "%Y-%m", &currentTime);

  inputPicker->web_view_.web_contents().DidReplaceDateTime(std::string(dateStr));
}

void InputPicker::_datetime_popup_response_cb(void* data,  Evas_Object* obj, void* event_info) {
  struct tm currentTime;
  memset(&currentTime, 0, sizeof(struct tm));

  InputPicker* inputPicker = static_cast<InputPicker*>(data);

  elm_datetime_value_get(inputPicker->picker_layout_->datePicker, &currentTime);
  mktime(&currentTime);

  char dateStr[50] = { 0, };
  if (inputPicker->picker_layout_->datetimeLocal)
    strftime(dateStr, 50, "%FT%R", &currentTime);
  else
    strftime(dateStr, 50, "%FT%RZ", &currentTime);

  inputPicker->web_view_.web_contents().DidReplaceDateTime(std::string(dateStr));
  inputPicker->web_view_.ExecuteEditCommand("Unselect", 0);

  inputPicker->deletePopupLayout();
  delete inputPicker->picker_layout_;
  inputPicker->picker_layout_ = 0;
}

#ifdef OS_TIZEN_MOBILE
void InputPicker::_popup_back_cb(void* data, Evas_Object* obj, void* event_info) {
  removeDatetimePicker(data);
}
#endif

} // namespace content
