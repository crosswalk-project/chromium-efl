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

/**
 * @file    ewk_console_message.h
 * @brief   Describes the Console Message API.
 */

#ifndef ewk_console_message_h
#define ewk_console_message_h

#include <Eina.h>
#include "ewk_export.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Creates a type name for @a _Ewk_Console_Message. */
typedef struct _Ewk_Console_Message Ewk_Console_Message;

/// Creates a type name for Ewk_Console_Message_Level.
typedef enum {
    EWK_CONSOLE_MESSAGE_LEVEL_NULL,
    EWK_CONSOLE_MESSAGE_LEVEL_LOG,
    EWK_CONSOLE_MESSAGE_LEVEL_WARNING,
    EWK_CONSOLE_MESSAGE_LEVEL_ERROR,
    EWK_CONSOLE_MESSAGE_LEVEL_DEBUG,
    EWK_CONSOLE_MESSAGE_LEVEL_INFO,
} Ewk_Console_Message_Level;

/**
 * Returns the log severity of the console message from the Console Message object
 *
 * @param message console message object
 *
 * @return Ewk_Console_Message_Level indicating the console message level,
           LogMessageLevel = 1,
           WarningMessageLevel = 2,
           ErrorMessageLevel = 3,
           DebugMessageLevel = 4,
           InfoMessageLevel = 5
 */
EAPI Ewk_Console_Message_Level ewk_console_message_level_get(const Ewk_Console_Message *message);

/**
 * Returns the console message text from the Console Message object
 *
 * @param message console message object
 *
 * @return console message text on success or empty string on failure
 */
EAPI Eina_Stringshare* ewk_console_message_text_get(const Ewk_Console_Message *message);

/**
 * Returns line no of the console message from the Console Message object
 *
 * @param message console message object
 *
 * @return the line number of the message on success or 0 on failure
 */
EAPI unsigned ewk_console_message_line_get(const Ewk_Console_Message *message);

/**
 * Returns the source of the console message from the Console Message object
 *
 * @param message console message object
 *
 * @return source of the console message on success or empty string on failure
 */
EAPI Eina_Stringshare* ewk_console_message_source_get(const Ewk_Console_Message *message);

#ifdef __cplusplus
}
#endif

#endif // ewk_console_message_h
