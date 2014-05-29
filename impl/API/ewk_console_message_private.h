/*
 * Copyright (C) 2014 Samsung Electronics
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef ewk_console_message_private_h
#define ewk_console_message_private_h

#include <Eina.h>

struct _Ewk_Console_Message {
  unsigned level;
  Eina_Stringshare* message;
  unsigned line;
  Eina_Stringshare* source;

  _Ewk_Console_Message(unsigned lvl, const char* message, unsigned int line_number, const char* source)
      : level(lvl),
        message(eina_stringshare_add(message)),
        line(line_number),
        source(eina_stringshare_add(source)) {
  }

  ~_Ewk_Console_Message() {
    eina_stringshare_del(message);
    eina_stringshare_del(source);
  }

};

#endif //ewk_console_message_private_h
