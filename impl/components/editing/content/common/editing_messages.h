// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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
