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

// TODO: move this file to other location than ewk's public

#include "public/text_encoding_map_efl.h"

// TODO: remove strings.h because it's non-standard.
#include <strings.h>

#include <string>

namespace{

  // The below Encodings have been taken from
  // chromium/src/third_party/WebKit/Source/wtf/text/TextCodecLatin1.cpp
  // chromium/src/third_party/WebKit/Source/wtf/text/TextCodecUTF8.cpp
  // chromium/src/third_party/WebKit/Source/wtf/text/TextCodecUserDefined.cpp
  // chromium/src/third_party/WebKit/Source/wtf/text/TextCodecUTF16.cpp
  // chromium/src/third_party/WebKit/Source/wtf/text/TextCodecICU.cpp
  //
  // The TextEncodingNameMap(present in /chromium/src/third_party/WebKit/Source/wtf/text/TextEncodingRegistry.cpp)
  // is contructed by combining the Encodings and aliases present in the above files.
  //
  // The character encoding list used by chrome browser is also used as reference. It is located in :
  // chromium/src/chrome/browser/character_encoding.cc

const std::string TextEncodingNameMap[] = {
"windows-1252",
"ISO-8859-1",
"US-ASCII",
"UTF-8",
"UTF-16LE",
"UTF-16BE",
"x-user-defined",
"ISO-8859-8-I",
"GBK",
"GB_2312-80",
"EUC-KR",
"Windows-1254",
"Windows-874",
"Big5",
"macintosh",
"EUC-JP",
"Big5-HKSCS",
"Shift_JIS",
"EUC-JP",
"ISO-2022-JP",
"windows-874",
"windows-949",
"ISO-8859-15",
"ISO-8859-2",
"windows-1250",
"windows-1251",
"windows-1258",
"KOI8-R",
"ISO-8859-7",
"windows-1253",
"windows-1256",
"windows-1257",
"ISO-8859-8",
"windows-1254",
"windows-1255",
"ISO-8859-6",
"ISO-8859-1",
"ISO-8859-3",
"ISO-8859-4",
"ISO-8859-10",
"ISO-8859-13",
"ISO-8859-14",
"ISO-8859-5",
"ISO-8859-9",
"cp864",
"x-mac-cyrillic"
};

} //namespace

const size_t maxEncodingNameLength = 63;
const size_t textEncodingNameMapLength = 46;

//static
bool TextEncodingMapEfl::isTextEncodingValid(const char* name)
{
 if (!name || !name[0] || (std::string(name).length() > maxEncodingNameLength))
  return 0;

 for (size_t i = 0; i < textEncodingNameMapLength; i++) {
   if (!strcasecmp(TextEncodingNameMap[i].c_str(), name))
    return true;
 }
 return false;
}
