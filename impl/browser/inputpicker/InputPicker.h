// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
    explicit InputPicker(EWebView& web_view_);
    ~InputPicker();

    void showDatePicker(
        tizen_webview::Input_Type input_type, double input_date);
    void showColorPicker(int r, int g, int b, int alpha);
    void hideColorPicker();

  private:
    struct Layout;

    void showDatePopup(double input_date);
    void showTimePopup(double input_date);
    void showMonthPopup(double input_date);
    void showWeekPopup(double input_date);
    void showDatetimePopup(double input_date, bool datetimeLocal);
    void removeDatetimePickerDelayed();
    void createPopupLayout(const char* title, struct tm* currentTime);
    void deletePopupLayout();
    void addColorRect(
        const char* part, int r, int g, int b, ColorPopupUserData* color_data);

    static void datePopupCallback(
        void* data, Evas_Object* obj, void* event_info);
    static void timePopupCallback(
        void* data, Evas_Object* obj, void* event_info);
    static void monthPopupCallback(
        void* data, Evas_Object* obj, void* event_info);
    static void weekPopupCallback(
        void* data, Evas_Object* obj, void* event_info);
    static void datetimePopupCallback(
        void* data, Evas_Object* obj, void* event_info);
    static void endEditingCallback(
        void* data, Evas_Object* obj, void* event_info);
    static Eina_Bool removeDatetimePicker(void* data);
    static void colorPickerCallback(
        void* data, Evas_Object* obj, void* event_info);
    static void selectedColorCallback(
        void* data, Evas* evas, Evas_Object* obj, void* event_info);
    static void keyDownColorPickerCallback(
        void* data, Evas* evas, Evas_Object* obj, void* event_info);
#if defined(OS_TIZEN_MOBILE)
    static void handleBackKeyColorPicker(
        void* data,  Evas_Object* obj, void* event_info);
    static void handleBackKeyDatePicker(
        void* data, Evas_Object* obj, void* event_info);
#endif

    EWebView& web_view_;
    Evas_Object* ewk_view_;
    Layout* picker_layout_;
    Eina_List* data_list_;
};

} // namespace content

#endif // InputPicker_h
