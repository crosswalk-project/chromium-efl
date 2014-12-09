// Copyright (c) 2014 Samsung Electronics All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "browser/speech/tts_message_filter_efl.h"

#include "base/bind.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/render_process_host.h"

namespace content {

TtsMessageFilterEfl::TtsMessageFilterEfl()
    : BrowserMessageFilter(TtsMsgStart) {
  tts_tizen_.reset(new TtsTizen(this));
}

TtsMessageFilterEfl::~TtsMessageFilterEfl() {
}

bool TtsMessageFilterEfl::Init(){
  return tts_tizen_->Init();
}

void TtsMessageFilterEfl::OverrideThreadForMessage(
    const IPC::Message& message, BrowserThread::ID* thread) {
  switch (message.type()) {
    case TtsHostMsg_InitializeVoiceList::ID:
    case TtsHostMsg_Speak::ID:
    case TtsHostMsg_Pause::ID:
    case TtsHostMsg_Resume::ID:
    case TtsHostMsg_Cancel::ID:
      *thread = BrowserThread::UI;
      break;
    default:
      NOTREACHED();
  }
}

bool TtsMessageFilterEfl::OnMessageReceived(const IPC::Message& message) {
  bool handled = true;
  IPC_BEGIN_MESSAGE_MAP(TtsMessageFilterEfl, message)
    IPC_MESSAGE_HANDLER(TtsHostMsg_InitializeVoiceList, OnInitializeVoiceList)
    IPC_MESSAGE_HANDLER(TtsHostMsg_Speak, OnSpeak)
    IPC_MESSAGE_HANDLER(TtsHostMsg_Pause, OnPause)
    IPC_MESSAGE_HANDLER(TtsHostMsg_Resume, OnResume)
    IPC_MESSAGE_HANDLER(TtsHostMsg_Cancel, OnCancel)
    IPC_MESSAGE_UNHANDLED(handled = false)
  IPC_END_MESSAGE_MAP()
  return handled;
}

void TtsMessageFilterEfl::OnChannelClosing() {
  NOTIMPLEMENTED();
}

void TtsMessageFilterEfl::OnInitializeVoiceList() {
  const std::vector<TtsVoice>& voices = tts_tizen_->GetVoiceList();
  Send(new TtsMsg_SetVoiceList(voices));
}

void TtsMessageFilterEfl::OnSpeak(const TtsUtteranceRequest& request) {
  tts_tizen_->Speak(request);
}

void TtsMessageFilterEfl::OnPause() {
  tts_tizen_->Pause();
}

void TtsMessageFilterEfl::OnResume() {
  tts_tizen_->Resume();
}

void TtsMessageFilterEfl::OnCancel() {
  tts_tizen_->Cancel();
}

void TtsMessageFilterEfl::DidFinishSpeaking(int utteranceid) {
  Send(new TtsMsg_DidFinishSpeaking(utteranceid));
}

void TtsMessageFilterEfl::DidResumeSpeaking(int utteranceid) {
  Send(new TtsMsg_DidResumeSpeaking(utteranceid));
}

void TtsMessageFilterEfl::DidPauseSpeaking(int utteranceid) {
  Send(new TtsMsg_DidPauseSpeaking(utteranceid));
}

void TtsMessageFilterEfl::DidStartSpeaking(int utteranceid) {
  Send(new TtsMsg_DidStartSpeaking(utteranceid));
}

void TtsMessageFilterEfl::SpeakingErrorOccurred(int utteranceid, const std::string& reason) {
  Send(new TtsMsg_SpeakingErrorOccurred(utteranceid, reason));
}

}  // namespace content
