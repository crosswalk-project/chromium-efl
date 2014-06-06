/*
   Copyright (C) 2013 Samsung Electronics

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

#ifndef AUTOFILL_POPUP_EFL_H
#define AUTOFILL_POPUP_EFL_H

#ifdef TIZEN_AUTOFILL_SUPPORT

#include <Ecore.h>
#include <Eina.h>
#include <Evas.h>
#include <Edje.h>
#include <Elementary.h>
#include <Ecore_X.h>
#include "ui/gfx/screen.h"
#include "components/autofill/core/browser/autofill_popup_delegate.h"
#include "browser/password_manager/password_form_manager.h"
#include "ui/gfx/image/image_skia.h"
#include "ui/gfx/point.h"

class EWebView;

namespace autofill {

class AutofillPopupViewEfl {
public:
  AutofillPopupViewEfl(EWebView* view);
  ~AutofillPopupViewEfl();
  void Show();
  void Hide();
  void ShowSavePasswordPopup(PasswordFormManager * form_to_save);
  void UpdateFormDataPopup(const gfx::RectF& bounds);
  void InitFormData(
        const std::vector<base::string16>& values,
        const std::vector<base::string16>& labels,
        const std::vector<base::string16>& icons,
        const std::vector<int>& identifiers,
        base::WeakPtr<AutofillPopupDelegate> delegate);
  void AcceptSuggestion(size_t index);
  void AcceptPasswordSuggestion(int option);
  void SetSelectedLine(size_t index);
  static char* getItemLabel(void* data, Evas_Object* obj, const char* part);
  static void itemSelectCb(void* data, Evas_Object* obj, void* event_info);
  static void savePasswordNeverCb(void *data, Evas_Object *obj, void *event_info);
  static void savePasswordYesCb(void *data, Evas_Object *obj, void *event_info);
  static void savePasswordNotNowCb(void *data, Evas_Object *obj, void *event_info);
private:
  EWebView * webview_;
  Evas_Object* autofill_popup_;
  Evas_Object* autofill_list_;
  Evas_Object* password_popup_;
  std::vector<base::string16> values_;
  std::vector<base::string16> icons_;
  std::vector<int> identifiers_;
  size_t selected_line_;
  base::WeakPtr<AutofillPopupDelegate> delegate_;
  PasswordFormManager * form_manager_;
};

} //namespace autofill

#endif //TIZEN_AUTOFILL_SUPPORT

#endif // autofill_popup_efl_h
