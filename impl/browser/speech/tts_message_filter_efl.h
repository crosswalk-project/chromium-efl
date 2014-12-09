// Copyright (c) 2014 Samsung Electronics Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BROWSER_SPEECH_TTS_MESSAGE_FILTER_H_
#define BROWSER_SPEECH_TTS_MESSAGE_FILTER_H_

#include "base/memory/scoped_ptr.h"
#include "base/memory/weak_ptr.h"
#include "browser/speech/tts_tizen.h"
#include "common/tts_messages_efl.h"
#include "content/public/browser/browser_message_filter.h"

namespace content {
class TtsTizen;
class BrowserContext;

// Handle IPC message from browser process to renderer process.
class TtsMessageFilterEfl
    : public content::BrowserMessageFilter,
      public base::SupportsWeakPtr<TtsMessageFilterEfl> {
 public:
  TtsMessageFilterEfl();
  bool Init();
  // content::BrowserMessageFilter implementation.
  void OverrideThreadForMessage(
      const IPC::Message& message,
      content::BrowserThread::ID* thread) override;
  bool OnMessageReceived(const IPC::Message& message) override;
  void OnChannelClosing() override;

  void DidFinishSpeaking(int utteranceid);
  void DidResumeSpeaking(int utteranceid);
  void DidPauseSpeaking(int utteranceid);
  void DidStartSpeaking(int utteranceid);
  void SpeakingErrorOccurred(int utteranceid, const std::string& reason);

 private:
  ~TtsMessageFilterEfl() override;

  void OnInitializeVoiceList();
  void OnSpeak(const TtsUtteranceRequest& utterance);
  void OnPause();
  void OnResume();
  void OnCancel();

  scoped_ptr<TtsTizen> tts_tizen_;

  DISALLOW_COPY_AND_ASSIGN(TtsMessageFilterEfl);
};

}  // namespace content

#endif  // BROWSER_SPEECH_TTS_MESSAGE_FILTER_H_
