// Copyright 2014 Samsung Electronics. All rights reseoved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/base/clipboard/clipboard.h"

#include <Elementary.h>

#include "base/logging.h"
#include "base/strings/string16.h"
#include "base/strings/utf_string_conversions.h"
#include "components/clipboard/clipboard_helper_efl.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "ui/gfx/size.h"

using base::string16;

namespace ui {
namespace {
const char kPlainTextFormat[] = "text";
const char kHTMLFormat[] = "html";
const char kRTFFormat[] = "rtf";
const char kBitmapFormat[] = "bitmap";
const char kWebKitSmartPasteFormat[] = "webkit_smart";
const char kBookmarkFormat[] = "bookmark";
const char kMimeTypePepperCustomData[] = "chromium/x-pepper-custom-data";
const char kMimeTypeWebCustomData[] = "chromium/x-web-custom-data";
} // namespace

Clipboard::FormatType::FormatType() {
}

Clipboard::FormatType::FormatType(const std::string& native_format)
    : data_(native_format) {
}

Clipboard::FormatType::~FormatType() {
}

std::string Clipboard::FormatType::Serialize() const {
  return data_;
}

// static
Clipboard::FormatType Clipboard::FormatType::Deserialize(
    const std::string& serialization) {
  return FormatType(serialization);
}

bool Clipboard::FormatType::Equals(const FormatType& other) const {
  return data_ == other.data_;
}

Clipboard::Clipboard() {
  DCHECK(CalledOnValidThread());
}

Clipboard::~Clipboard() {
  DCHECK(CalledOnValidThread());
}

// Main entry point used to write several values in the clipboard.
void Clipboard::WriteObjects(ClipboardType type, const ObjectMap& objects) {
  DCHECK(CalledOnValidThread());
  DCHECK_EQ(type, CLIPBOARD_TYPE_COPY_PASTE);

  for (ObjectMap::const_iterator iter = objects.begin();
       iter != objects.end(); ++iter) {
    DispatchObject(static_cast<ObjectType>(iter->first), iter->second);
  }
}

uint64 Clipboard::GetSequenceNumber(ClipboardType /* type */) {
  DCHECK(CalledOnValidThread());
  // TODO: implement this. For now this interface will advertise
  // that the clipboard never changes. That's fine as long as we
  // don't rely on this signal.
  return 0;
}

bool Clipboard::IsFormatAvailable(const Clipboard::FormatType& format,
                                  ClipboardType /* clipboard_type */) const {
  DCHECK(CalledOnValidThread());
  int count = ClipboardHelperEfl::GetInstance()->NumberOfItems();

  int type = ELM_SEL_FORMAT_NONE;
  std::string clipboard_data;

  for (int i = 0; i < count; i++) {
    bool ret = ClipboardHelperEfl::GetInstance()->RetrieveClipboardItem(i, &type, &clipboard_data);
    if (!ret)
      continue;

    switch (type) {
      case ELM_SEL_FORMAT_TEXT: {
        if (GetPlainTextFormatType().Equals(format))
          return true;
        break;
      }
      case ELM_SEL_FORMAT_HTML: {
        if (GetHtmlFormatType().Equals(format))
          return true;
        break;
      }
      case ELM_SEL_FORMAT_IMAGE: {
        if (GetBitmapFormatType().Equals(format))
          return true;
        break;
      }
      // If we support more ELM formats, we shall add it here.
    }
  }
  return false;
}

void Clipboard::Clear(ClipboardType /* type */) {
  ClipboardHelperEfl::GetInstance()->Clear();
}

void Clipboard::ReadAvailableTypes(ClipboardType type, std::vector<string16>* types,
                                   bool* contains_filenames) const {
  DCHECK(CalledOnValidThread());
  DCHECK_EQ(type, CLIPBOARD_TYPE_COPY_PASTE);

  if (!types || !contains_filenames) {
    NOTREACHED();
    return;
  }

  NOTIMPLEMENTED();

  types->clear();
  *contains_filenames = false;
}

void Clipboard::ReadText(ClipboardType type, string16* result) const {
  DCHECK(CalledOnValidThread());
  DCHECK_EQ(type, CLIPBOARD_TYPE_COPY_PASTE);
  std::string utf8;
  ReadAsciiText(type, &utf8);
  *result = base::UTF8ToUTF16(utf8);
}

void Clipboard::ReadAsciiText(ClipboardType type, std::string* result) const {
  DCHECK(CalledOnValidThread());
  DCHECK_EQ(type, CLIPBOARD_TYPE_COPY_PASTE);
  int count = ClipboardHelperEfl::GetInstance()->NumberOfItems();

  int format = ELM_SEL_FORMAT_NONE;
  std::string clipboard_data;

  for (int i = 0; i < count; i++) {
    if (!ClipboardHelperEfl::GetInstance()->RetrieveClipboardItem(i, &format, &clipboard_data))
      continue;

    if (format == ELM_SEL_FORMAT_TEXT) {
      result->clear();
      result->assign(clipboard_data);
      break;
    }
  }
}

void Clipboard::ReadHTML(ClipboardType type, string16* markup,
    std::string* src_url, uint32* fragment_start,
    uint32* fragment_end) const {
  DCHECK(CalledOnValidThread());
  DCHECK_EQ(type, CLIPBOARD_TYPE_COPY_PASTE);
  int count = ClipboardHelperEfl::GetInstance()->NumberOfItems();
  if (src_url)
    src_url->clear();
  // This is supposed to contain context for html parsing.
  // We set 0 and markup->size() respectively, which is same as other platforms.
  *fragment_start = 0;
  *fragment_end = 0;

  int format = ELM_SEL_FORMAT_NONE;
  std::string clipboard_data;

  for (int i = 0; i < count; i++) {
    if (!ClipboardHelperEfl::GetInstance()->RetrieveClipboardItem(i, &format, &clipboard_data))
      continue;

    if (format == ELM_SEL_FORMAT_HTML) {
      markup->clear();
      markup->assign(base::UTF8ToUTF16(clipboard_data));
      *fragment_end = static_cast<uint32>(markup->size());
      break;
    }
  }
}

void Clipboard::ReadRTF(ClipboardType type, std::string* result) const {
  NOTIMPLEMENTED();
}

SkBitmap Clipboard::ReadImage(ClipboardType type) const {
  DCHECK(CalledOnValidThread());
  DCHECK_EQ(type, CLIPBOARD_TYPE_COPY_PASTE);
  NOTIMPLEMENTED();

  return SkBitmap();
}

void Clipboard::ReadBookmark(string16* title, std::string* url) const {
  NOTIMPLEMENTED();
}

void Clipboard::ReadCustomData(ClipboardType clipboard_type,
                               const string16& type,
                               string16* result) const {
  NOTIMPLEMENTED();
}

void Clipboard::ReadData(const FormatType& format, std::string* result) const {
  DCHECK(CalledOnValidThread());
  NOTIMPLEMENTED();
}

// static
Clipboard::FormatType Clipboard::GetFormatType(const std::string& format_string) {
  return FormatType::Deserialize(format_string);
}

// static
const Clipboard::FormatType& Clipboard::GetPlainTextFormatType() {
  CR_DEFINE_STATIC_LOCAL(FormatType, type, (kPlainTextFormat));
  return type;
}

// static
const Clipboard::FormatType& Clipboard::GetPlainTextWFormatType() {
  CR_DEFINE_STATIC_LOCAL(FormatType, type, (kPlainTextFormat));
  return type;
}

// static
const Clipboard::FormatType& Clipboard::GetWebKitSmartPasteFormatType() {
  CR_DEFINE_STATIC_LOCAL(FormatType, type, (kWebKitSmartPasteFormat));
  return type;
}

// static
const Clipboard::FormatType& Clipboard::GetHtmlFormatType() {
  CR_DEFINE_STATIC_LOCAL(FormatType, type, (kHTMLFormat));
  return type;
}

// static
const Clipboard::FormatType& Clipboard::GetRtfFormatType() {
  CR_DEFINE_STATIC_LOCAL(FormatType, type, (kRTFFormat));
  return type;
}

// static
const Clipboard::FormatType& Clipboard::GetBitmapFormatType() {
  CR_DEFINE_STATIC_LOCAL(FormatType, type, (kBitmapFormat));
  return type;
}

// static
const Clipboard::FormatType& Clipboard::GetWebCustomDataFormatType() {
  CR_DEFINE_STATIC_LOCAL(FormatType, type, (kMimeTypeWebCustomData));
  return type;
}

// static
const Clipboard::FormatType& Clipboard::GetPepperCustomDataFormatType() {
  CR_DEFINE_STATIC_LOCAL(FormatType, type, (kMimeTypePepperCustomData));
  return type;
}

void Clipboard::WriteText(const char* text_data, size_t text_len) {
  ClipboardHelperEfl::GetInstance()->
      SetData(std::string(text_data, text_len), ClipboardHelperEfl::CLIPBOARD_DATA_TYPE_PLAIN_TEXT);
}

void Clipboard::WriteHTML(const char* markup_data, size_t markup_len,
                          const char* url_data, size_t url_len) {
  NOTIMPLEMENTED();
}

void Clipboard::WriteRTF(const char* rtf_data, size_t data_len) {
  NOTIMPLEMENTED();
}

void Clipboard::WriteBookmark(const char* title_data, size_t title_len,
                              const char* url_data, size_t url_len) {
  NOTIMPLEMENTED();
}

void Clipboard::WriteWebSmartPaste() {
  NOTIMPLEMENTED();
}

void Clipboard::WriteBitmap(const SkBitmap& bitmap) {
  NOTIMPLEMENTED();
}

void Clipboard::WriteData(const FormatType& format, const char* data_data, size_t data_len) {
  NOTIMPLEMENTED();
}

} //namespace ui
