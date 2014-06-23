/*
    Copyright (C) 2014 Samsung Electronics

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB. If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "browser/renderer_host/render_message_filter_efl.h"

#include "common/web_contents_utils.h"
#include "common/render_messages_efl.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/web_contents.h"
#include "net/url_request/url_request_context_getter.h"
#include "web_contents_delegate_efl.h"

using web_contents_utils::WebContentsFromViewID;
using content::BrowserThread;

RenderMessageFilterEfl::RenderMessageFilterEfl(int render_process_id)
  : BrowserMessageFilter(ChromeMsgStart)
  , render_process_id_(render_process_id) {
}

RenderMessageFilterEfl::~RenderMessageFilterEfl() { }

void RenderMessageFilterEfl::OverrideThreadForMessage(const IPC::Message& message,
                                                      content::BrowserThread::ID* thread)
{
  switch (message.type()) {
  case EwkHostMsg_DecideNavigationPolicy::ID:
    *thread = content::BrowserThread::UI;
    break;
  }
}

bool RenderMessageFilterEfl::OnMessageReceived(const IPC::Message& message) {
  bool handled = true;
  IPC_BEGIN_MESSAGE_MAP(RenderMessageFilterEfl, message)
    IPC_MESSAGE_HANDLER(EwkHostMsg_DecideNavigationPolicy, OnDecideNavigationPolicy)
    IPC_MESSAGE_UNHANDLED(handled = false)
  IPC_END_MESSAGE_MAP()
  return handled;
}

void RenderMessageFilterEfl::OnDecideNavigationPolicy(NavigationPolicyParams params, bool* handled) {
  DCHECK(BrowserThread::CurrentlyOn(BrowserThread::UI));

  if (content::WebContents* web_contents =
      WebContentsFromViewID(render_process_id_, params.render_view_id)) {
    content::WebContentsDelegateEfl* delegate =
    static_cast<content::WebContentsDelegateEfl*>(web_contents->GetDelegate());
    // delegate replies directly
    delegate->web_view()->InvokePolicyNavigationCallback(web_contents->GetRenderViewHost(), params, handled);
  }
}

