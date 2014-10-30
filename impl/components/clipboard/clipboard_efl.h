// Copyright 2014 Samsung Electronics. All rights reseoved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CLIPBOARD_EFL_H
#define CLIPBOARD_EFL_H

#include "ui/base/clipboard/clipboard.h"
namespace ui {

class ClipboardEfl: public Clipboard {
 private:
  friend class Clipboard;

  ClipboardEfl();
  ~ClipboardEfl() override;

  // Clipboard overrides:
  uint64 GetSequenceNumber(ClipboardType type) override;
  bool IsFormatAvailable(const FormatType& format,
                         ClipboardType type) const override;
  void Clear(ClipboardType type) override;
  void ReadAvailableTypes(ClipboardType type,
                          std::vector<base::string16>* types,
                          bool* contains_filenames) const override;
  void ReadText(ClipboardType type, base::string16* result) const override;
  void WriteObjects(ClipboardType type, const ObjectMap& objects) override;
  void ReadAsciiText(ClipboardType type, std::string* result) const override;
  void ReadHTML(ClipboardType type,
                base::string16* markup,
                std::string* src_url,
                uint32* fragment_start,
                uint32* fragment_end) const override;
  void ReadRTF(ClipboardType type, std::string* result) const override;
  SkBitmap ReadImage(ClipboardType type) const override;
  void ReadBookmark(base::string16* title, std::string* url) const override;
  void ReadCustomData(ClipboardType clipboard_type,
                      const base::string16& type,
                      base::string16* result) const override;
  void ReadData(const FormatType& format, std::string* result) const override;
  void WriteText(const char* text_data, size_t text_len) override;

  void WriteHTML(const char* markup_data,
                 size_t markup_len,
                 const char* url_data,
                 size_t url_len) override;

  void WriteRTF(const char* rtf_data, size_t data_len) override;

  void WriteBookmark(const char* title_data,
                     size_t title_len,
                     const char* url_data,
                     size_t url_len) override;

  void WriteWebSmartPaste() override;

  void WriteBitmap(const SkBitmap& bitmap) override;

  void WriteData(const FormatType& format,
                 const char* data_data,
                 size_t data_len) override;
};

} // namespace ui

#endif /* CLIPBOARD_EFL_H_ */
