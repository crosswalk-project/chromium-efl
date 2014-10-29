// Copyright 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "browser/renderer_host/render_message_filter_efl.h"

#include "common/web_contents_utils.h"
#include "common/render_messages_efl.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/render_view_host.h"
#include "content/public/browser/web_contents.h"
#include "net/url_request/url_request_context_getter.h"
#include "web_contents_delegate_efl.h"

#if defined(TIZEN_MULTIMEDIA_SUPPORT)
#include "content/browser/media/tizen/webaudio_decoder_browser_gstreamer.h"
#endif

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
#if defined(TIZEN_MULTIMEDIA_SUPPORT)
    IPC_MESSAGE_HANDLER(EflViewHostMsg_GstWebAudioDecode, OnGstWebAudioDecode)
#endif
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

#if defined(TIZEN_MULTIMEDIA_SUPPORT)
void RenderMessageFilterEfl::OnGstWebAudioDecode(
    base::SharedMemoryHandle encoded_data_handle,
    base::FileDescriptor pcm_output,
    uint32_t data_size) {
  content::BrowserMessageFilterTizen::GetInstance()->EncodedDataReceived(
      encoded_data_handle, pcm_output, data_size);
}
#endif
