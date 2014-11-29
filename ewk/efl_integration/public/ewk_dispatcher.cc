/*
 * Copyright (C) 2013 Samsung Electronics
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "ewk_dispatcher.h"
#include <assert.h>

// // TODO: remove dependency to chromium internal
#include <base/message_loop/message_loop_proxy.h>
#include <base/bind.h>
#include <base/location.h>

using namespace base;

static scoped_refptr<MessageLoopProxy> g_message_loop;

void ewk_dispatcher_touch()
{
  // Check: "Message loop initialized twice."
  //DCHECK(!g_message_loop);
  g_message_loop = MessageLoopProxy::current();
}

void ewk_dispatcher_dispatch(ewk_dispatch_callback cb, void *user_data, unsigned delay)
{
  // Check: "Message loop not initialized."
  //DCHECK(!!g_message_loop);
  // Check: "Callback not provided."
  //DCHECK(!!cb);

  if (delay) {
    g_message_loop->PostDelayedTask(FROM_HERE,
                                    Bind(cb, user_data),
                                    TimeDelta::FromMilliseconds(delay));
  } else {
    g_message_loop->PostTask(FROM_HERE,
                             Bind(cb, user_data));
  }
}


