// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
