// Copyright 2014 Samsung Electronics. All rights reseoved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CLIPBOARD_HELPER_EFL_H_
#define CLIPBOARD_HELPER_EFL_H_

#include <string>
#include <Ecore.h>
#include <Ecore_X.h>

#include "base/basictypes.h"

// This class is based on ClipboardHelper class in WK2/Tizen.
// Coding Style has been changed as per Chromium coding style.

// TODO: Get some documentation how this works with CBHM.
// CBHM is an application/daemon running in Tizen device.
// This helper class interacts with CBHM using Ecore/X message passing API.
// There are a few magic strings in implementation.
class EWebView;
template <typename T> struct DefaultSingletonTraits;
class ClipboardHelperEfl {
 public:
  enum ClipboardDataType {
    CLIPBOARD_DATA_TYPE_PLAIN_TEXT,
    CLIPBOARD_DATA_TYPE_URI_LIST,
    CLIPBOARD_DATA_TYPE_URL,
    CLIPBOARD_DATA_TYPE_MARKUP,
    CLIPBOARD_DATA_TYPE_IMAGE
  };
  static ClipboardHelperEfl* GetInstance();

  void SetData(const std::string& data, ClipboardDataType type);
  void Clear();
  static int NumberOfItems();
  bool RetrieveClipboardItem(int index, int* format, std::string* data);
  void OpenClipboardWindow(EWebView* view, bool richly_editable);
  void CloseClipboardWindow();
  bool IsClipboardWindowOpened();

 private:
  ClipboardHelperEfl();
  friend struct DefaultSingletonTraits<ClipboardHelperEfl>;

  Ecore_X_Window GetCbhmWindow();
  bool SendCbhmMessage(const std::string& message);
  bool SetClipboardItem(Ecore_X_Atom data_type, const std::string& data);
  std::string GetCbhmReply(Ecore_X_Window xwin, Ecore_X_Atom property, Ecore_X_Atom* data_type);
  DISALLOW_COPY_AND_ASSIGN(ClipboardHelperEfl);

};

#endif /* CLIPBOARD_HELPER_EFL_H_ */
