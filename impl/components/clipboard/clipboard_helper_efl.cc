// Copyright 2014 Samsung Electronics. All rights reseoved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/clipboard/clipboard_helper_efl.h"
#include "eweb_view.h"

#include <Evas.h>
#include <Ecore_Evas.h>
#include <Ecore_X.h>
#include <Elementary.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>

#include "base/memory/singleton.h"

namespace {

const char kCbhmMessageHide[] = "cbhm_hide";
const char kCbhmMessageSetItem[] = "SET_ITEM";
const char kCbhmMessageShow0[] = "show0";
const char kCbhmMessageShow1[] = "show1";

const char kCbhmAtomClipboardCount[] = "CBHM_cCOUNT";
const char kCbhmAtomElmMarkup[] = "application/x-elementary-markup";
const char kCbhmAtomError[] = "CBHM_ERROR";
const char kCbhmAtomItem[] = "CBHM_ITEM";
const char kCbhmAtomMessage[] = "CBHM_MSG";
const char kCbhmAtomTextHtml[] = "text/html;charset=utf-8";
const char kCbhmAtomTextUri[] = "text/uri";
const char kCbhmAtomUTF8String[] = "UTF8_STRING";
const char kCbhmAtomXWindow[] = "CBHM_XWIN";

const int kDataUnitSize8 = 8;
const int kDataUnitSize16 = 16;
const int kDataUnitSize32 = 32;
}  // namespace

ClipboardHelperEfl* ClipboardHelperEfl::GetInstance() {
  return Singleton<ClipboardHelperEfl>::get();
}

ClipboardHelperEfl::ClipboardHelperEfl() {
}

void ClipboardHelperEfl::SetData(const std::string& data, ClipboardDataType type) {
  Ecore_X_Atom data_type = 0;

  if (type == CLIPBOARD_DATA_TYPE_PLAIN_TEXT ||
      type == CLIPBOARD_DATA_TYPE_URI_LIST ||
      type == CLIPBOARD_DATA_TYPE_URL) {
    data_type = ecore_x_atom_get(kCbhmAtomUTF8String);
  } else {
    //TODO: Other formats need to be added here.
    NOTIMPLEMENTED();
    return;
  }

  SetClipboardItem(data_type, data);
}

void ClipboardHelperEfl::Clear() {
  NOTIMPLEMENTED();
}

Ecore_X_Window ClipboardHelperEfl::GetCbhmWindow() {
  Ecore_X_Atom xAtomCbhm = ecore_x_atom_get(kCbhmAtomXWindow);
  Ecore_X_Window xCbhmWin = 0;

  unsigned char* buf = 0;
  int property_size = 0;
  int property_array_size = ecore_x_window_prop_property_get(0, xAtomCbhm, XA_WINDOW, 0, &buf, &property_size);

  if (property_array_size && property_size)
    memcpy(&xCbhmWin, buf, sizeof(Ecore_X_Window));

  if (buf)
    free(buf);

  return xCbhmWin;
}

bool ClipboardHelperEfl::SendCbhmMessage(const std::string& message) {
  Ecore_X_Window cbhm_win = GetCbhmWindow();
  Ecore_X_Atom atom_cbhm_item = ecore_x_atom_get(kCbhmAtomMessage);

  if (!cbhm_win || !atom_cbhm_item)
    return false;
  Ecore_X_Window xwin = ecore_x_window_focus_get();
  XClientMessageEvent messageEvent;
  memset(&messageEvent, 0, sizeof(messageEvent));
  messageEvent.type = ClientMessage;
  messageEvent.display = static_cast<Display*>(ecore_x_display_get());
  messageEvent.window = xwin;
  messageEvent.message_type = atom_cbhm_item;
  messageEvent.format = 8; /* data should be viewed as byte */
  snprintf(messageEvent.data.b, sizeof(messageEvent.data.b), "%s", message.c_str());

  XSendEvent(static_cast<Display*>(ecore_x_display_get()), cbhm_win, false, NoEventMask,
             reinterpret_cast<XEvent*>(&messageEvent));

  ecore_x_sync();

  return true;
}

bool ClipboardHelperEfl::SetClipboardItem(Ecore_X_Atom data_type,const std::string& data) {
  Ecore_X_Window cbhm_win = GetCbhmWindow();
  Ecore_X_Atom atom_cbhm_item = ecore_x_atom_get(kCbhmAtomItem);

  ecore_x_sync();
  ecore_x_window_prop_property_set(cbhm_win,
                                   atom_cbhm_item,
                                   data_type,
                                   8,
                                   const_cast<char*>(data.c_str()),
                                   data.length() + 1);
  ecore_x_sync();

  if (SendCbhmMessage(kCbhmMessageSetItem))
    return true;

  return false;
}

std::string ClipboardHelperEfl::GetCbhmReply(Ecore_X_Window xwin, Ecore_X_Atom property, Ecore_X_Atom* data_type) {
  if (!property)
    return std::string();

  ecore_x_sync();

  Ecore_X_Atom type;
  int data_unit_size = 0;
  long unsigned int data_length = 0;
  long unsigned int bytes = 0;
  unsigned char* data = 0;

  int result = XGetWindowProperty(static_cast<Display*>(ecore_x_display_get()),
                                  xwin, property, 0, LONG_MAX, False, ecore_x_window_prop_any_type(),
                                  reinterpret_cast<Atom*>(&type), &data_unit_size, &data_length,
                                  &bytes, &data);

  if (result != Success)
    return std::string();

  if (!data_length) {
    XFree(data);
    return std::string();
  }

  std::string cbhm_data;

  switch (data_unit_size) {
  case kDataUnitSize8:
    cbhm_data = std::string(reinterpret_cast<const char*>(data), data_length);
    break;
  case kDataUnitSize16:
    cbhm_data = UTF16ToUTF8(base::string16(reinterpret_cast<const base::char16*>(data), data_length));
    break;
  case kDataUnitSize32:
    break;
  default:
    NOTREACHED();
    break;
  }

  XFree(data);

  if (data_type)
    *data_type = type;

  return cbhm_data;
}

bool ClipboardHelperEfl::RetrieveClipboardItem(int index, int* format, std::string* data) {
  if (!data)
    return false;

  Ecore_X_Window cbhm_win = GetCbhmWindow();
  std::ostringstream cbhm_item;
  cbhm_item << kCbhmAtomItem << index;

  Ecore_X_Atom atom_cbhm_item = ecore_x_atom_get(cbhm_item.str().c_str());
  Ecore_X_Atom atom_item_type = 0;

  std::string result = GetCbhmReply(cbhm_win, atom_cbhm_item, &atom_item_type);
  if (result.empty() || atom_item_type == ecore_x_atom_get(kCbhmAtomError))
    return false;

  //TODO: Other formats need to be added here.
  if ((atom_item_type == ecore_x_atom_get(kCbhmAtomUTF8String)) ||
      (atom_item_type == ecore_x_atom_get(kCbhmAtomElmMarkup))) {
    *format = ELM_SEL_FORMAT_TEXT;
    data->swap(result);
    return true;
  }
  return false;
}

int ClipboardHelperEfl::NumberOfItems() {
  // 1. Get CBHM Ecore_X_Window.
  Ecore_X_Atom x_atom;
  x_atom = ecore_x_atom_get(kCbhmAtomXWindow);
  if (!x_atom)
    return 0;

  unsigned char* data = 0;
  int number_of_windows = 0;
  int result = ecore_x_window_prop_property_get(0, x_atom, XA_WINDOW, 0, &data, &number_of_windows);

  Ecore_X_Window x_window = 0;
  if (result && number_of_windows)
    memcpy(&x_window, data, sizeof(Ecore_X_Window));

  if (data)
    free(data);

  if (!x_window)
    return 0;

  ecore_x_sync();

  // 2. Get number of clipboard items.
  x_atom = ecore_x_atom_get(kCbhmAtomClipboardCount);
  if (!x_atom)
    return 0;

  Display* display = static_cast<Display*>(ecore_x_display_get());

  Ecore_X_Atom type;
  int format;
  long unsigned number_of_items = 0;
  long unsigned bytes = 0;
  unsigned char* data_in_format = 0;
  result = XGetWindowProperty(display, x_window, x_atom, 0, LONG_MAX, False,
                              ecore_x_window_prop_any_type(), reinterpret_cast<Atom*>(&type),
                              &format, &number_of_items, &bytes, &data_in_format);
  if (result != Success)
    return 0;

  if (!number_of_items) {
    XFree(data_in_format);
    return 0;
  }

  if (!data_in_format)
    return 0;

  char* end;
  errno = 0;
  long int count = strtol(reinterpret_cast<const char*>(data_in_format), &end, 10);
  if ((end == reinterpret_cast<const char*>(data_in_format) || *end != '\0') /* not a number */||
      ((count == LONG_MIN || count == LONG_MAX) && ERANGE == errno) /* out of range */) {
    DLOG(ERROR) << "String to int conversion failed.";
    return 0;
  }
  return count;
}

void ClipboardHelperEfl::OpenClipboardWindow(EWebView* view, bool richly_editable) {
  if (richly_editable)
    SendCbhmMessage(kCbhmMessageShow1);
  else
    SendCbhmMessage(kCbhmMessageShow0);

  view->SmartCallback<EWebViewCallbacks::ClipboardOpened>().call(0);
}

void ClipboardHelperEfl::CloseClipboardWindow() {
  SendCbhmMessage(kCbhmMessageHide);
}
