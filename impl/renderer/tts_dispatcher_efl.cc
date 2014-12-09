// Copyright (c) 2014 Samsung Electronics Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "renderer/tts_dispatcher_efl.h"

#include "base/logging.h"
#include "base/basictypes.h"
#include "base/strings/utf_string_conversions.h"
#include "common/tts_messages_efl.h"
#include "common/tts_utterance_request_efl.h"
#include "content/public/renderer/render_thread.h"
#include "third_party/WebKit/public/platform/WebCString.h"
#include "third_party/WebKit/public/platform/WebSpeechSynthesisUtterance.h"
#include "third_party/WebKit/public/platform/WebSpeechSynthesisVoice.h"
#include "third_party/WebKit/public/platform/WebString.h"
#include "third_party/WebKit/public/platform/WebVector.h"

namespace content {
int TtsDispatcherEfl::next_utterance_id_ = 1;

TtsDispatcherEfl::TtsDispatcherEfl(blink::WebSpeechSynthesizerClient* client)
    : synthesizer_client_(client) {
  content::RenderThread::Get()->AddObserver(this);
}

TtsDispatcherEfl::~TtsDispatcherEfl() {
  content::RenderThread::Get()->RemoveObserver(this);
}

bool TtsDispatcherEfl::OnControlMessageReceived(const IPC::Message& message) {
  IPC_BEGIN_MESSAGE_MAP(TtsDispatcherEfl, message)
    IPC_MESSAGE_HANDLER(TtsMsg_SetVoiceList, OnSetVoiceList)
    IPC_MESSAGE_HANDLER(TtsMsg_DidStartSpeaking, OnDidStartSpeaking)
    IPC_MESSAGE_HANDLER(TtsMsg_DidFinishSpeaking, OnDidFinishSpeaking)
    IPC_MESSAGE_HANDLER(TtsMsg_DidPauseSpeaking, OnDidPauseSpeaking)
    IPC_MESSAGE_HANDLER(TtsMsg_DidResumeSpeaking, OnDidResumeSpeaking)
    IPC_MESSAGE_HANDLER(TtsMsg_WordBoundary, OnWordBoundary)
    IPC_MESSAGE_HANDLER(TtsMsg_SentenceBoundary, OnSentenceBoundary)
    IPC_MESSAGE_HANDLER(TtsMsg_MarkerEvent, OnMarkerEvent)
    IPC_MESSAGE_HANDLER(TtsMsg_WasInterrupted, OnWasInterrupted)
    IPC_MESSAGE_HANDLER(TtsMsg_WasCancelled, OnWasCancelled)
    IPC_MESSAGE_HANDLER(TtsMsg_SpeakingErrorOccurred, OnSpeakingErrorOccurred)
  IPC_END_MESSAGE_MAP()

  // Always return false because there may be multiple TtsDispatchers
  // and we want them all to have a chance to handle this message.
  return false;
}

void TtsDispatcherEfl::updateVoiceList() {
  content::RenderThread::Get()->Send(new TtsHostMsg_InitializeVoiceList());
}

void TtsDispatcherEfl::speak(const blink::WebSpeechSynthesisUtterance& web_utterance) {
  int id = next_utterance_id_++;

  utterance_id_map_[id] = web_utterance;

  TtsUtteranceRequest utterance;
  utterance.id = id;
  utterance.text = web_utterance.text().utf8();
  if (!web_utterance.lang().isEmpty() && web_utterance.lang().at(2) == '-')
    utterance.lang = web_utterance.lang().utf8().replace(2,1,"_");
  utterance.voice = web_utterance.voice().utf8();
  utterance.volume = web_utterance.volume();
  utterance.rate = web_utterance.rate();
  utterance.pitch = web_utterance.pitch();
  content::RenderThread::Get()->Send(new TtsHostMsg_Speak(utterance));
}

void TtsDispatcherEfl::pause() {
  content::RenderThread::Get()->Send(new TtsHostMsg_Pause());
}

void TtsDispatcherEfl::resume() {
  content::RenderThread::Get()->Send(new TtsHostMsg_Resume());
}

void TtsDispatcherEfl::cancel() {
  content::RenderThread::Get()->Send(new TtsHostMsg_Cancel());
}

blink::WebSpeechSynthesisUtterance TtsDispatcherEfl::FindUtterance(int utterance_id) {
  base::hash_map<int, blink::WebSpeechSynthesisUtterance>::const_iterator iter =
      utterance_id_map_.find(utterance_id);
  if (iter == utterance_id_map_.end())
    return blink::WebSpeechSynthesisUtterance();
  return iter->second;
}

void TtsDispatcherEfl::OnSetVoiceList(const std::vector<TtsVoice>& voices) {
  blink::WebVector<blink::WebSpeechSynthesisVoice> out_voices(voices.size());
  for (size_t i = 0; i < voices.size(); ++i) {
    out_voices[i].setVoiceURI(blink::WebString::fromUTF8(voices[i].voice_uri));
    out_voices[i].setName(blink::WebString::fromUTF8(voices[i].name));
    out_voices[i].setLanguage(blink::WebString::fromUTF8(voices[i].lang));
    out_voices[i].setIsLocalService(voices[i].local_service);
    out_voices[i].setIsDefault(voices[i].is_default);
  }
  synthesizer_client_->setVoiceList(out_voices);
}

void TtsDispatcherEfl::OnDidStartSpeaking(int utterance_id) {
  if (utterance_id_map_.find(utterance_id) == utterance_id_map_.end())
    return;

  blink::WebSpeechSynthesisUtterance utterance = FindUtterance(utterance_id);
  if (utterance.isNull())
    return;

  synthesizer_client_->didStartSpeaking(utterance);
}

void TtsDispatcherEfl::OnDidFinishSpeaking(int utterance_id) {
  blink::WebSpeechSynthesisUtterance utterance = FindUtterance(utterance_id);
  if (utterance.isNull())
    return;

  synthesizer_client_->didFinishSpeaking(utterance);
  utterance_id_map_.erase(utterance_id);
}

void TtsDispatcherEfl::OnDidPauseSpeaking(int utterance_id) {
  blink::WebSpeechSynthesisUtterance utterance = FindUtterance(utterance_id);
  if (utterance.isNull())
    return;

  synthesizer_client_->didPauseSpeaking(utterance);
}

void TtsDispatcherEfl::OnDidResumeSpeaking(int utterance_id) {
  blink::WebSpeechSynthesisUtterance utterance = FindUtterance(utterance_id);
  if (utterance.isNull())
    return;

  synthesizer_client_->didResumeSpeaking(utterance);
}

void TtsDispatcherEfl::OnWordBoundary(int utterance_id, int char_index) {
  CHECK(char_index >= 0);

  blink::WebSpeechSynthesisUtterance utterance = FindUtterance(utterance_id);
  if (utterance.isNull())
    return;

  synthesizer_client_->wordBoundaryEventOccurred(
      utterance, static_cast<unsigned>(char_index));
}

void TtsDispatcherEfl::OnSentenceBoundary(int utterance_id, int char_index) {
  CHECK(char_index >= 0);

  blink::WebSpeechSynthesisUtterance utterance = FindUtterance(utterance_id);
  if (utterance.isNull())
    return;

  synthesizer_client_->sentenceBoundaryEventOccurred(
      utterance, static_cast<unsigned>(char_index));
}

void TtsDispatcherEfl::OnMarkerEvent(int utterance_id, int char_index) {
  // Not supported yet.
}

void TtsDispatcherEfl::OnWasInterrupted(int utterance_id) {
  blink::WebSpeechSynthesisUtterance utterance = FindUtterance(utterance_id);
  if (utterance.isNull())
    return;

  // The web speech API doesn't support "interrupted".
  synthesizer_client_->didFinishSpeaking(utterance);
  utterance_id_map_.erase(utterance_id);
}

void TtsDispatcherEfl::OnWasCancelled(int utterance_id) {
  blink::WebSpeechSynthesisUtterance utterance = FindUtterance(utterance_id);
  if (utterance.isNull())
    return;

  // The web speech API doesn't support "cancelled".
  synthesizer_client_->didFinishSpeaking(utterance);
  utterance_id_map_.erase(utterance_id);
}

void TtsDispatcherEfl::OnSpeakingErrorOccurred(int utterance_id,
                                            const std::string& error_message) {
  blink::WebSpeechSynthesisUtterance utterance = FindUtterance(utterance_id);
  if (utterance.isNull())
    return;

  // The web speech API doesn't support an error message.
  synthesizer_client_->speakingErrorOccurred(utterance);
  utterance_id_map_.erase(utterance_id);
}

}  // namespace content
