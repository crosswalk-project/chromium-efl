/*
 * Copyright (C) 2013 Samsung Electronics. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 */

// Multiply-included message file, hence no include guard.

#include "content/public/common/common_param_traits.h"
#include "content/public/common/common_param_traits_macros.h"
#include "ipc/ipc_message_macros.h"
#include "ipc/ipc_message_utils.h"

#if defined(EWK_BRINGUP)
#define IPC_MESSAGE_START LastIPCMsgStart
#else
#define IPC_MESSAGE_START EditingMsgStart
#endif

// Editor messages sent from the renderer to the browser.

// Notification message carrying undo stack size after executing undo
IPC_MESSAGE_ROUTED2(EditingHostMsg_NotifyUndo,
                    int, /* render view ID */
                    size_t /* size of the undo stack */)

// Notification message carrying redo stack size after executing redo
IPC_MESSAGE_ROUTED2(EditingHostMsg_NotifyRedo,
                    int, /* render view ID */
                    size_t /* size of the redo stack */)

// Notification message carrying undo stack size after registering undo
IPC_MESSAGE_ROUTED2(EditingHostMsg_NotifyRegisterUndo,
                    int, /* render view ID */
                    size_t /* size of the undo stack */)

// Notification message carrying redo stack size after registering redo
IPC_MESSAGE_ROUTED2(EditingHostMsg_NotifyRegisterRedo,
                    int, /* render view ID */
                    size_t /* size of the redo stack */)

// Notification message carrying undo stack size after clearing undo stack
IPC_MESSAGE_ROUTED2(EditingHostMsg_NotifyClearUndo,
                    int, /* render view ID */
                    size_t /* size of the undo stack */)

// Notification message carrying redo stack size after clearing redo stack
IPC_MESSAGE_ROUTED2(EditingHostMsg_NotifyClearRedo,
                    int, /* render view ID */
                    size_t /* size of the redo stack */)
