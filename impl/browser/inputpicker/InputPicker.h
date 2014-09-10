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

#ifndef InputPicker_h
#define InputPicker_h

#include <ctime>
#include <Evas.h>

#include "tizen_webview/public/tw_input_type.h"

class EWebView;

namespace content {

class WebContents;
struct ColorPopupUserData;
class WebContentsDelegateEfl;

class InputPicker {
  public:
    InputPicker(EWebView&);
    ~InputPicker();

    void show(tizen_webview::Input_Type, const char*);
    void showColorPicker(int, int, int, int);
    void hideColorPicker();

    void ewk_date_popup(const char*);
    void ewk_time_popup(const char*);
    void ewk_month_popup(const char*);
    void ewk_week_popup(const char*);
    void ewk_datetime_popup(const char*, bool);
    void ewk_color_popup(int, int, int);
    void removeDatetimePickerDelayed();

  private:
    struct Layout;

    void createDatetimePopup(const char*, struct tm*);
    void createDatetimePicker(struct tm*);
    void deletePopupLayout();
    void addColorRect(const char*, int, int, int, ColorPopupUserData*);

    static void _date_popup_response_cb(void*, Evas_Object*, void*);
    static void _time_popup_response_cb(void*, Evas_Object*, void*);
    static void _month_popup_response_cb(void*, Evas_Object*, void*);
    static void _week_popup_response_cb(void*, Evas_Object*, void*);
    static void _datetime_popup_response_cb(void*, Evas_Object*, void*);
    static void _color_popup_response_cb(void*, Evas_Object*, void*);
    static void _data_list_popup_response_cb(void*, Evas_Object*, void*);
    static void _data_list_popup_response_cancel_cb(void*, Evas_Object*, void*);
    static void _data_list_selected_cb(void*, Evas_Object*, void*);
    static void _color_selected_cb(void*, Evas*, Evas_Object*, void*);
    static void _edit_end_cb(void* data, Evas_Object* obj, void* event_info);
    static void colorKeyDownCallback(void*, Evas*, Evas_Object*, void*);

#ifdef OS_TIZEN_MOBILE
    static void _color_back_cb(void*, Evas_Object*, void*);
    static void _popup_back_cb(void*, Evas_Object*, void*);
#endif
    static Eina_Bool removeDatetimePicker(void*);

    EWebView& web_view_;
    Evas_Object* ewk_view_;
    Layout* picker_layout_;
    Eina_List* data_list_;
};

} // namespace content

#endif // InputPicker_h
