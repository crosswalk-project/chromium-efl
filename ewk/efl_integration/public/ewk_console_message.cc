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

#include "ewk_console_message.h"

// TODO: remove dependency
#include <../impl/API/ewk_console_message_private.h>

Ewk_Console_Message_Level ewk_console_message_level_get(const Ewk_Console_Message *message)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(message, EWK_CONSOLE_MESSAGE_LEVEL_NULL);
  return static_cast<Ewk_Console_Message_Level>(message->level);
}

Eina_Stringshare* ewk_console_message_text_get(const Ewk_Console_Message *message)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(message, 0);
  return message->message;
}

unsigned ewk_console_message_line_get(const Ewk_Console_Message *message)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(message, 0);
  return message->line;
}

Eina_Stringshare* ewk_console_message_source_get(const Ewk_Console_Message *message)
{
  EINA_SAFETY_ON_NULL_RETURN_VAL(message, 0);
  return message->source;
}
