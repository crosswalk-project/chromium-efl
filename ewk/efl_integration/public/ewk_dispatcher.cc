// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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


