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

enum ClipType
{
  CLIP_TYPE_PRIMARY,
  CLIP_TYPE_SECONDARY,
  CLIP_TYPE_CLIPBOARD,
  CLIP_TYPE_XDND,
  CLIP_TYPE_MAX,
};

struct ClipData
{
  Ecore_X_Selection selection;
  void (* request)(Ecore_X_Window window, const char* pTarget);

  Elm_Sel_Format format;
  Elm_Sel_Format requestedFormat;
  int bufferLength;

  ClipboardHelperEfl* clipboardHelper;
  EWebView* webView;
};

typedef int (*ClipNotifyHandler)(ClipData* clipData, Ecore_X_Event_Selection_Notify* notifyData);

enum
{
  ATOM_TARGETS = 0,
  ATOM_ATOM,
  ATOM_LISTING_ATOMS = ATOM_ATOM,
  ATOM_TEXT_URI,
  ATOM_TEXT_URILIST,
  ATOM_TEXT_X_VCARD,
  ATOM_IMAGE_PNG,
  ATOM_IMAGE_JPEG,
  ATOM_IMAGE_BMP,
  ATOM_IMAGE_GIF,
  ATOM_IMAGE_TIFF,
  ATOM_IMAGE_SVG,
  ATOM_IMAGE_XPM,
  ATOM_IMAGE_TGA,
  ATOM_IMAGE_PPM,
  ATOM_XELM,
  ATOM_TEXT_HTML_UTF8,
  ATOM_TEXT_HTML,
  ATOM_STRING_UTF8,
  ATOM_STRING,
  ATOM_TEXT,
  ATOM_TEXT_PLAIN_UTF8,
  ATOM_TEXT_PLAIN,
  ATOM_FORMAT_NONE,
  ATOM_MAX,
};

struct AtomData
{
  const char* pName;
  Elm_Sel_Format formats;
  ClipNotifyHandler notify;
  Ecore_X_Atom atom;
};

AtomData atomList[ATOM_MAX];
ClipData clipList[CLIP_TYPE_MAX];

Eina_Bool propertyChangeCallback(void* data, int type, void* event) {
  ClipboardHelperEfl *clipboardHelper = static_cast<ClipboardHelperEfl*>(data);
  Ecore_X_Event_Window_Property *ev = (Ecore_X_Event_Window_Property*) event;

  if (ev->atom == ECORE_X_ATOM_E_ILLUME_CLIPBOARD_STATE)
    clipboardHelper->UpdateClipboardWindowState(ev);

  return EINA_TRUE;
}

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
const char kCbhmAtomTextUriList[] = "text/uri-list";
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
  m_selectionClearHandler = 0;
  m_selectionNotifyHandler = 0;
  property_change_handler_ = 0;
  clipboard_window_opened_ = false;
}

void ClipboardHelperEfl::SetData(const std::string& data, ClipboardDataType type) {
  Ecore_X_Atom data_type = 0;

  switch(type)
  {
    case CLIPBOARD_DATA_TYPE_PLAIN_TEXT:
    case CLIPBOARD_DATA_TYPE_URI_LIST:
    case CLIPBOARD_DATA_TYPE_URL:
      data_type = ecore_x_atom_get(kCbhmAtomUTF8String);
      break;
    case CLIPBOARD_DATA_TYPE_IMAGE:
      data_type = ecore_x_atom_get(kCbhmAtomTextUriList);
      break;
    case CLIPBOARD_DATA_TYPE_MARKUP:
      data_type = ecore_x_atom_get(kCbhmAtomTextHtml);
    default:
      NOTIMPLEMENTED();
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
  if (data.empty())
    return false;

  Ecore_X_Window cbhm_win = GetCbhmWindow();
  Ecore_X_Atom atom_cbhm_item = ecore_x_atom_get(kCbhmAtomItem);

  size_t data_start_index = 0;
  const char prefix[] = "file://";
  if (data.compare(0, sizeof(prefix) - 1, prefix) == 0)
    data_start_index = sizeof(prefix) - 1;

  ecore_x_sync();
  ecore_x_window_prop_property_set(cbhm_win,
                                   atom_cbhm_item,
                                   data_type,
                                   8,
                                   const_cast<char*>(data.c_str()) + data_start_index,
                                   data.length() - data_start_index + 1);
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
    if (atom_item_type == ecore_x_atom_get(kCbhmAtomElmMarkup))
      *data = std::string(evas_textblock_text_markup_to_utf8(NULL, data->c_str()));
    return true;
  } else if (atom_item_type == ecore_x_atom_get(kCbhmAtomTextHtml)) {
    *format = ELM_SEL_FORMAT_HTML;
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

static void pasteSelectedClipboardItem(std::string data, std::string type, EWebView *webview) {
  content::SelectionControllerEfl* controller = webview->GetSelectionController();
  if (controller)
    controller->HideHandleAndContextMenu();

  webview->ExecuteEditCommand(type.c_str(), data.c_str());
  ClipboardHelperEfl::connectClipboardWindow();
}

static Eina_Bool clearClip(void* data, int type, void* event) {
  Ecore_X_Event_Selection_Clear* clearEvent = (Ecore_X_Event_Selection_Clear*) event;
  Ecore_X_Window window = clearEvent->win;

  ClipType clipType = CLIP_TYPE_SECONDARY;
  clipList[clipType].requestedFormat = static_cast<Elm_Sel_Format>(ELM_SEL_FORMAT_TEXT | ELM_SEL_FORMAT_IMAGE);

  ecore_x_selection_secondary_request(window, ECORE_X_SELECTION_TARGET_TARGETS);
  return ECORE_CALLBACK_PASS_ON;
}

static Eina_Bool notifyClip(void* data , int type, void* event) {
  EWebView *webview = static_cast<EWebView*>(data);
  Ecore_X_Event_Selection_Notify* notifytEvent = (Ecore_X_Event_Selection_Notify*) event;

  int i = 0;
  for (i = 0; i < CLIP_TYPE_MAX; i++) {
    if (clipList[i].selection == notifytEvent->selection)
      break;
  }

  ClipData* clipData = clipList + i;
  clipData->webView = webview;
  for (i = 0; i < ATOM_MAX; i++) {
    if (!strcmp(notifytEvent->target, atomList[i].pName) && atomList[i].notify)
      atomList[i].notify(clipData, notifytEvent);
  }
  return ECORE_CALLBACK_PASS_ON;
}

static int notifyTarget(ClipData* clipData, Ecore_X_Event_Selection_Notify* notifyData) {
  Ecore_X_Atom dataType = 0;

  if (clipData->clipboardHelper->getSelectedCbhmItem(&dataType)) {
    const char* pHtmlAtomName = "text/html;charset=utf-8";
    Ecore_X_Atom htmlType = ecore_x_atom_get(pHtmlAtomName);

    if (dataType == htmlType) {
      clipData->request(notifyData->win, pHtmlAtomName);
      return ECORE_CALLBACK_PASS_ON;
    }
  }

  Ecore_X_Selection_Data_Targets* pTargets = (Ecore_X_Selection_Data_Targets*) (notifyData->data);
  Ecore_X_Atom* pAtomList = (Ecore_X_Atom*) (pTargets->data.data);

  int i, j = 0;
  for (j = (ATOM_LISTING_ATOMS+1); j < ATOM_MAX; j++) {
    if (!(atomList[j].formats & clipData->requestedFormat))
      continue;
    for (i = 0; i < pTargets->data.length; i++) {
      if ((atomList[j].atom == pAtomList[i]) && (atomList[j].notify)) {
        if ((j == ATOM_XELM) && (!(clipData->requestedFormat & ELM_SEL_FORMAT_MARKUP)))
          continue;
        goto FOUND;
      }
    }
  }

  return ECORE_CALLBACK_PASS_ON;

  FOUND:
  clipData->request(notifyData->win, atomList[j].pName);
  return ECORE_CALLBACK_PASS_ON;
}

static int notifyText(ClipData* clipData, Ecore_X_Event_Selection_Notify* notifyData) {
  Ecore_X_Selection_Data* pData = (Ecore_X_Selection_Data*) notifyData->data;
  pasteSelectedClipboardItem(std::string((char*)pData->data), "InsertText", clipData->webView);
  return 0;
}

static int notifyImage(ClipData* clipData, Ecore_X_Event_Selection_Notify* notifyData) {
  return 0;
}

static int notifyUri(ClipData* clipData,
    Ecore_X_Event_Selection_Notify* notifyData) {
  Ecore_X_Selection_Data* pData = (Ecore_X_Selection_Data*) notifyData->data;
  pasteSelectedClipboardItem(
      std::string(reinterpret_cast<char*>(pData->data), pData->length),
      "InsertImage", clipData->webView);
}

static int notifyEdje(ClipData* clipData, Ecore_X_Event_Selection_Notify* notifyData) {
  return 0;
}

static int notifyHtml(ClipData* clipData,
    Ecore_X_Event_Selection_Notify* notifyData) {
  Ecore_X_Selection_Data* pData = (Ecore_X_Selection_Data*) notifyData->data;
  pasteSelectedClipboardItem(
      std::string(reinterpret_cast<char*>(pData->data), pData->length),
      "InsertHTML", clipData->webView);
  return 0;
}

void ClipboardHelperEfl::initializeAtomList() {
  atomList[ATOM_TARGETS].pName = "TARGETS";
  atomList[ATOM_TARGETS].formats = ELM_SEL_FORMAT_TARGETS;
  atomList[ATOM_TARGETS].notify = notifyTarget;
  atomList[ATOM_TARGETS].atom = 0;

  atomList[ATOM_ATOM].pName = "ATOM";
  atomList[ATOM_ATOM].formats = ELM_SEL_FORMAT_TARGETS;
  atomList[ATOM_ATOM].notify = notifyTarget;
  atomList[ATOM_ATOM].atom = 0;

  atomList[ATOM_XELM].pName = "application/x-elementary-markup";
  atomList[ATOM_XELM].formats = ELM_SEL_FORMAT_MARKUP;
  atomList[ATOM_XELM].notify = notifyEdje;
  atomList[ATOM_XELM].atom = 0;

  atomList[ATOM_TEXT_URI].pName = "text/uri";
  atomList[ATOM_TEXT_URI].formats = ELM_SEL_FORMAT_IMAGE;
  atomList[ATOM_TEXT_URI].notify = notifyUri;
  atomList[ATOM_TEXT_URI].atom = 0;

  atomList[ATOM_TEXT_URILIST].pName = "text/uri-list";
  atomList[ATOM_TEXT_URILIST].formats = ELM_SEL_FORMAT_IMAGE;
  atomList[ATOM_TEXT_URILIST].notify = notifyUri;
  atomList[ATOM_TEXT_URILIST].atom = 0;

  atomList[ATOM_TEXT_X_VCARD].pName = "text/x-vcard";
  atomList[ATOM_TEXT_X_VCARD].formats = ELM_SEL_FORMAT_VCARD;
  atomList[ATOM_TEXT_X_VCARD].notify = 0;
  atomList[ATOM_TEXT_X_VCARD].atom = 0;

  atomList[ATOM_IMAGE_PNG].pName = "image/png";
  atomList[ATOM_IMAGE_PNG].formats = ELM_SEL_FORMAT_IMAGE;
  atomList[ATOM_IMAGE_PNG].notify = notifyImage;
  atomList[ATOM_IMAGE_PNG].atom = 0;

  atomList[ATOM_IMAGE_JPEG].pName = "image/jpeg";
  atomList[ATOM_IMAGE_JPEG].formats = ELM_SEL_FORMAT_IMAGE;
  atomList[ATOM_IMAGE_JPEG].notify = notifyImage;
  atomList[ATOM_IMAGE_JPEG].atom = 0;

  atomList[ATOM_IMAGE_BMP].pName = "image/x-ms-bmp";
  atomList[ATOM_IMAGE_BMP].formats = ELM_SEL_FORMAT_IMAGE;
  atomList[ATOM_IMAGE_BMP].notify = notifyImage;
  atomList[ATOM_IMAGE_BMP].atom = 0;

  atomList[ATOM_IMAGE_GIF].pName = "image/gif";
  atomList[ATOM_IMAGE_GIF].formats = ELM_SEL_FORMAT_IMAGE;
  atomList[ATOM_IMAGE_GIF].notify = notifyImage;
  atomList[ATOM_IMAGE_GIF].atom = 0;

  atomList[ATOM_IMAGE_TIFF].pName = "image/tiff";
  atomList[ATOM_IMAGE_TIFF].formats = ELM_SEL_FORMAT_IMAGE;
  atomList[ATOM_IMAGE_TIFF].notify = notifyImage;
  atomList[ATOM_IMAGE_TIFF].atom = 0;

  atomList[ATOM_IMAGE_SVG].pName = "image/svg+xml";
  atomList[ATOM_IMAGE_SVG].formats = ELM_SEL_FORMAT_IMAGE;
  atomList[ATOM_IMAGE_SVG].notify = notifyImage;
  atomList[ATOM_IMAGE_SVG].atom = 0;

  atomList[ATOM_IMAGE_XPM].pName = "image/x-xpixmap";
  atomList[ATOM_IMAGE_XPM].formats = ELM_SEL_FORMAT_IMAGE;
  atomList[ATOM_IMAGE_XPM].notify = notifyImage;
  atomList[ATOM_IMAGE_XPM].atom = 0;

  atomList[ATOM_IMAGE_TGA].pName = "image/x-tga";
  atomList[ATOM_IMAGE_TGA].formats = ELM_SEL_FORMAT_IMAGE;
  atomList[ATOM_IMAGE_TGA].notify = notifyImage;
  atomList[ATOM_IMAGE_TGA].atom = 0;

  atomList[ATOM_IMAGE_PPM].pName = "image/x-portable-pixmap";
  atomList[ATOM_IMAGE_PPM].formats = ELM_SEL_FORMAT_IMAGE;
  atomList[ATOM_IMAGE_PPM].notify = notifyImage;
  atomList[ATOM_IMAGE_PPM].atom = 0;

  atomList[ATOM_TEXT_HTML_UTF8].pName = "text/html;charset=utf-8";
  atomList[ATOM_TEXT_HTML_UTF8].formats = ELM_SEL_FORMAT_HTML;
  atomList[ATOM_TEXT_HTML_UTF8].notify = notifyHtml;
  atomList[ATOM_TEXT_HTML_UTF8].atom = 0;

  atomList[ATOM_TEXT_HTML].pName = "text/html";
  atomList[ATOM_TEXT_HTML].formats = ELM_SEL_FORMAT_HTML;
  atomList[ATOM_TEXT_HTML].notify = notifyHtml;
  atomList[ATOM_TEXT_HTML].atom = 0;

  atomList[ATOM_STRING_UTF8].pName = "UTF8_STRING";
  atomList[ATOM_STRING_UTF8].formats = static_cast<Elm_Sel_Format> (ELM_SEL_FORMAT_TEXT| ELM_SEL_FORMAT_MARKUP| ELM_SEL_FORMAT_HTML);
  atomList[ATOM_STRING_UTF8].notify = notifyText;
  atomList[ATOM_STRING_UTF8].atom = 0;

  atomList[ATOM_STRING].pName = "STRING";
  atomList[ATOM_STRING].formats = static_cast<Elm_Sel_Format> (ELM_SEL_FORMAT_TEXT| ELM_SEL_FORMAT_MARKUP| ELM_SEL_FORMAT_HTML);
  atomList[ATOM_STRING].notify = notifyText;
  atomList[ATOM_STRING].atom = 0;

  atomList[ATOM_TEXT].pName = "TEXT";
  atomList[ATOM_TEXT].formats = static_cast<Elm_Sel_Format> (ELM_SEL_FORMAT_TEXT| ELM_SEL_FORMAT_MARKUP| ELM_SEL_FORMAT_HTML);
  atomList[ATOM_TEXT].notify = 0;
  atomList[ATOM_TEXT].atom = 0;

  atomList[ATOM_TEXT_PLAIN_UTF8].pName = "text/plain;charset=utf-8";
  atomList[ATOM_TEXT_PLAIN_UTF8].formats = static_cast<Elm_Sel_Format> (ELM_SEL_FORMAT_TEXT| ELM_SEL_FORMAT_MARKUP| ELM_SEL_FORMAT_HTML);
  atomList[ATOM_TEXT_PLAIN_UTF8].notify = 0;
  atomList[ATOM_TEXT_PLAIN_UTF8].atom = 0;

  atomList[ATOM_TEXT_PLAIN].pName = "text/plain";
  atomList[ATOM_TEXT_PLAIN].formats = static_cast<Elm_Sel_Format> (ELM_SEL_FORMAT_TEXT| ELM_SEL_FORMAT_MARKUP| ELM_SEL_FORMAT_HTML);
  atomList[ATOM_TEXT_PLAIN].notify = notifyText;
  atomList[ATOM_TEXT_PLAIN].atom = 0;

  atomList[ATOM_FORMAT_NONE].pName = "NONE";
  atomList[ATOM_FORMAT_NONE].formats = ELM_SEL_FORMAT_NONE;
  atomList[ATOM_FORMAT_NONE].notify = notifyText;
  atomList[ATOM_FORMAT_NONE].atom = 0;

  clipList[CLIP_TYPE_PRIMARY].selection = ECORE_X_SELECTION_PRIMARY;
  clipList[CLIP_TYPE_PRIMARY].request = ecore_x_selection_primary_request;
  clipList[CLIP_TYPE_PRIMARY].bufferLength = 0;

  clipList[CLIP_TYPE_SECONDARY].selection = ECORE_X_SELECTION_SECONDARY;
  clipList[CLIP_TYPE_SECONDARY].request = ecore_x_selection_secondary_request;
  clipList[CLIP_TYPE_SECONDARY].bufferLength = 0;

  clipList[CLIP_TYPE_CLIPBOARD].selection = ECORE_X_SELECTION_CLIPBOARD;
  clipList[CLIP_TYPE_CLIPBOARD].request = ecore_x_selection_clipboard_request;
  clipList[CLIP_TYPE_CLIPBOARD].bufferLength = 0;

  clipList[CLIP_TYPE_XDND].selection = ECORE_X_SELECTION_XDND;
  clipList[CLIP_TYPE_XDND].request = ecore_x_selection_xdnd_request;
  clipList[CLIP_TYPE_XDND].bufferLength = 0;

  for (int i = 0; i < ATOM_MAX; i++)
    atomList[i].atom = ecore_x_atom_get(atomList[i].pName);
}

void ClipboardHelperEfl::clearClipboardHandler() {
  if (m_selectionClearHandler) {
    ecore_event_handler_del(m_selectionClearHandler);
    m_selectionClearHandler = 0;
  }

  if (m_selectionNotifyHandler) {
    ecore_event_handler_del(m_selectionNotifyHandler);
    m_selectionNotifyHandler = 0;
  }

  if (property_change_handler_) {
    ecore_event_handler_del(property_change_handler_);
    property_change_handler_ = 0;
  }
  clipboard_window_opened_ = false;
}

bool ClipboardHelperEfl::getSelectedCbhmItem(Ecore_X_Atom* pDataType) {
  Ecore_X_Window cbhmWin = GetCbhmWindow();
  Ecore_X_Atom atomCbhmItem = ecore_x_atom_get("CBHM_SELECTED_ITEM");
  Ecore_X_Atom atomItemType = 0;

  std::string result = GetCbhmReply(cbhmWin, atomCbhmItem, &atomItemType);

  if(result.empty())
    return false;

  if (atomItemType == ecore_x_atom_get("CBHM_ERROR"))
    return false;

  if (pDataType)
    *pDataType = atomItemType;

  return true;
}

void ClipboardHelperEfl::OpenClipboardWindow(EWebView* view, bool richly_editable) {
  clearClipboardHandler();
  initializeAtomList();
  property_change_handler_ = ecore_event_handler_add(
      ECORE_X_EVENT_WINDOW_PROPERTY, propertyChangeCallback, this);
  m_selectionClearHandler = ecore_event_handler_add(ECORE_X_EVENT_SELECTION_CLEAR, clearClip, view);
  m_selectionNotifyHandler = ecore_event_handler_add(ECORE_X_EVENT_SELECTION_NOTIFY, notifyClip, view);

  if (richly_editable)
    SendCbhmMessage(kCbhmMessageShow1);
  else
    SendCbhmMessage(kCbhmMessageShow0);

  connectClipboardWindow();
  view->SmartCallback<EWebViewCallbacks::ClipboardOpened>().call(0);
}

void ClipboardHelperEfl::connectClipboardWindow() {
  ecore_x_selection_secondary_set(ecore_x_window_focus_get(), "",1);
}

bool ClipboardHelperEfl::IsClipboardWindowOpened() {
  return clipboard_window_opened_;
}

void ClipboardHelperEfl::CloseClipboardWindow() {
  SendCbhmMessage(kCbhmMessageHide);
}

void ClipboardHelperEfl::UpdateClipboardWindowState(Ecore_X_Event_Window_Property* ev) {
  Ecore_X_Illume_Clipboard_State state = ecore_x_e_illume_clipboard_state_get(ev->win);

  if (state == ECORE_X_ILLUME_CLIPBOARD_STATE_OFF)
    clearClipboardHandler();
  else if (state == ECORE_X_ILLUME_CLIPBOARD_STATE_ON)
    clipboard_window_opened_ = true;
}
