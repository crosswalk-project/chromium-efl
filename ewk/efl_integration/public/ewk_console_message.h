// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
