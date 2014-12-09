// Copyright (c) 2014 Samsung Electronics Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "browser/speech/tts_tizen.h"

#include <vector>

#include "base/logging.h"

namespace content {
namespace {
const char kUriPrefix[] = "localhost:";
const std::string kAuto = "Auto";
const std::string kMale = "Male";
const std::string kFemale = "Female";
const std::string kChild = "Child";


std::string VoiceTypeToName(tts_voice_type_e type) {
  switch (type) {
    case TTS_VOICE_TYPE_AUTO:
      return kAuto;
    case TTS_VOICE_TYPE_MALE:
      return kMale;
    case TTS_VOICE_TYPE_FEMALE:
      return kFemale;
    case TTS_VOICE_TYPE_CHILD:
      return kChild;
  }
  return kAuto;
}

tts_voice_type_e VoiceNameToType(std::string name) {
  if (name == kAuto)
    return TTS_VOICE_TYPE_AUTO;
  else if (name == kMale)
    return TTS_VOICE_TYPE_MALE;
  else if (name == kFemale)
    return TTS_VOICE_TYPE_FEMALE;
  else if (name == kChild)
    return TTS_VOICE_TYPE_CHILD;
  else
    return TTS_VOICE_TYPE_AUTO;
}

const std::string ErrorToString(tts_error_e error) {
  switch (error) {
    case TTS_ERROR_NONE:
      return "Successful";
    case TTS_ERROR_OUT_OF_MEMORY:
      return "Out of Memory";
    case TTS_ERROR_IO_ERROR:
      return "I/O error";
    case TTS_ERROR_INVALID_PARAMETER:
      return "Invalid parameter";
    case TTS_ERROR_OUT_OF_NETWORK:
      return "Out of network";
    case TTS_ERROR_INVALID_STATE:
      return "Invalid state";
    case TTS_ERROR_INVALID_VOICE:
      return "Invalid voice";
    case TTS_ERROR_ENGINE_NOT_FOUND:
      return "No available engine";
    case TTS_ERROR_TIMED_OUT:
      return "No answer from the daemon";
    case TTS_ERROR_OPERATION_FAILED:
      return "Operation failed";
    case TTS_ERROR_AUDIO_POLICY_BLOCKED:
      return "Audio policy blocked";
    default:
      return "Unknown Error";
  }
}

void TtsStateChangedCallback(tts_h tts_handle, tts_state_e previous, tts_state_e current,
                        void* user_data) {
  TtsTizen* _this = static_cast<TtsTizen*>(user_data);

  if (TTS_STATE_CREATED == previous && TTS_STATE_READY == current)
    _this->TtsReady();
  else if (TTS_STATE_PAUSED == previous &&
       TTS_STATE_PLAYING == current && _this->GetUtterance().id)
    _this->GetTtsMessageFilterEfl()->DidResumeSpeaking(_this->GetUtterance().id);
  else if (TTS_STATE_PLAYING == previous &&
       TTS_STATE_PAUSED == current && _this->GetUtterance().id)
    _this->GetTtsMessageFilterEfl()->DidPauseSpeaking(_this->GetUtterance().id);
}

void TtsUtteranceStartedCallback(tts_h tts_handle, int utteranceId, void* user_data) {
  TtsTizen* _this = static_cast<TtsTizen*>(user_data);
  if (_this->GetUtterance().id)
    _this->GetTtsMessageFilterEfl()->
         DidStartSpeaking(_this->GetUtterance().id);
}

void TtsUtteranceCompletedCallback(tts_h tts_handle, int utteranceId, void *user_data) {
  TtsTizen* _this = static_cast<TtsTizen*>(user_data);
  if (_this->GetUtterance().id)
    _this->GetTtsMessageFilterEfl()->
      DidFinishSpeaking(_this->GetUtterance().id);
}

void TtsErrorCallback(tts_h tts_handle, int utteranceId, tts_error_e reason, void* user_data) {
  TtsTizen* _this = static_cast<TtsTizen*>(user_data);
  if (_this->GetUtterance().id)
    _this->GetTtsMessageFilterEfl()->
      SpeakingErrorOccurred(_this->GetUtterance().id, ErrorToString(reason));
}

bool TtsSupportedVoiceCallback(tts_h tts_handle,
                                const char* language,
                                int voice_type, void* user_data) {
  TtsTizen* _this = static_cast<TtsTizen*>(user_data);
  tts_voice_type_e currentVoiceType = static_cast<tts_voice_type_e>(voice_type);
  _this->AddVoice(std::string(language), currentVoiceType);
  return true;
}

bool InitializeTtsEngine(tts_h* tts_handle) {
  int ret = tts_create(tts_handle);
  if (ret != TTS_ERROR_NONE) {
    LOG(ERROR) << "Fail to create TTS: " << ErrorToString(static_cast<tts_error_e>(ret));
    *tts_handle = NULL;
    return false;
  }
  ret = tts_set_mode(*tts_handle, TTS_MODE_DEFAULT);
  if (ret != TTS_ERROR_NONE) {
    LOG(ERROR) << "Fail to set TTS mode: " << ErrorToString(static_cast<tts_error_e>(ret));
  }
  ret = tts_prepare(*tts_handle);
  if (ret != TTS_ERROR_NONE) {
    LOG(ERROR) << "Fail to prepare TTS: " << ErrorToString(static_cast<tts_error_e>(ret));
    *tts_handle = NULL;
    return false;
  }
  return true;
}

}  // namespace

TtsTizen::TtsTizen(TtsMessageFilterEfl* tts_message_filter_efl)
  : tts_message_filter_efl_(tts_message_filter_efl),
    tts_state_ready_(false),
    tts_handle_(NULL),
    default_voice_(TTS_VOICE_TYPE_AUTO),
    utterance_waiting_(false) {
}

TtsTizen::~TtsTizen() {
  int ret = tts_unset_state_changed_cb(tts_handle_) &&
    tts_unset_utterance_completed_cb(tts_handle_) &&
    tts_unset_error_cb(tts_handle_);
  if (ret != TTS_ERROR_NONE)
    LOG(ERROR) << "TTS: Fail to unset callbacks";

  ret = tts_unprepare(tts_handle_);
  if (ret != TTS_ERROR_NONE)
    LOG(ERROR) << "TTS: Fail to unprepare";

  tts_destroy(tts_handle_);
  tts_handle_ = NULL;
}

bool TtsTizen::Init() {
  tts_initialized_ = InitializeTtsEngine(&tts_handle_);
  if (!tts_initialized_) {
    return false;
 }

  SetTtsDefaultVoice();

  // Set callbacks
  SetTtsCallback(this);
  return true;
}

void TtsTizen::SetTtsDefaultVoice() {
  char* language = NULL;
  int voice_type;
  int ret = tts_get_default_voice(tts_handle_, &language, &voice_type);
  if (ret != TTS_ERROR_NONE) {
    LOG(ERROR) << "TTS: Fail to get default voice";
    // Even if ret != TTS_ERROR_NONE, do not return here.
    // Set the default values.
    const char kDefaultLanguage[] = "en_GB";
    default_language_ = kDefaultLanguage;
    default_voice_ = TTS_VOICE_TYPE_AUTO;
  } else {
    DCHECK(language);
    default_language_ = language;
    free(language);
    default_voice_ = static_cast<tts_voice_type_e>(voice_type);
  }
}


void TtsTizen::SetTtsCallback(void* tts_data) {
  int ret = tts_set_state_changed_cb(tts_handle_, TtsStateChangedCallback, tts_data);
  if (ret != TTS_ERROR_NONE)
    LOG(ERROR) << "TTS: Unable to set state callback";

  ret = tts_set_utterance_started_cb(tts_handle_, TtsUtteranceStartedCallback, tts_data);
  if (ret != TTS_ERROR_NONE)
    LOG(ERROR) << "TTS: Fail to set utterance started callback";

  ret = tts_set_utterance_completed_cb(tts_handle_, TtsUtteranceCompletedCallback, tts_data);
  if (ret != TTS_ERROR_NONE)
    LOG(ERROR) << "TTS: Fail to set utterance completed callback";

  ret = tts_set_error_cb(tts_handle_, TtsErrorCallback, this);
  if (ret != TTS_ERROR_NONE)
    LOG(ERROR) << "TTS: Fail to set error callback";

  ret = tts_foreach_supported_voices(tts_handle_, TtsSupportedVoiceCallback, this);
  if (ret != TTS_ERROR_NONE)
    LOG(ERROR) << "TTS: Fail to get supported voices";
}

const std::vector<TtsVoice>& TtsTizen::GetVoiceList() {
  return voice_list_;
}

void TtsTizen::TtsReady() {
  tts_state_ready_ = true;
  if (utterance_waiting_)
    SpeakStoredUtterance();
  utterance_waiting_ = false;
}

void TtsTizen::Speak(const TtsUtteranceRequest& utterance) {
  utterance_ = utterance;
  if (!tts_state_ready_) {
    utterance_waiting_ = true;
    return;
  }
  SpeakStoredUtterance();
}

void TtsTizen::SpeakStoredUtterance() {
  if (utterance_.text.empty()) {
    return;
  }
  std::string current_language = default_language_;
  if (!utterance_.lang.empty())
    current_language = utterance_.lang;

  int voiceType = static_cast<int>(default_voice_);
  if (!utterance_.voice.empty())
    voiceType = static_cast<int>(VoiceNameToType(utterance_.voice));
  tts_speed_e textSpeed = SetVoiceSpeed(utterance_.rate);

  int utteranceId = utterance_.id;
  int ret = tts_add_text(tts_handle_, utterance_.text.c_str(), current_language.c_str(),
                         voiceType, textSpeed, &utteranceId);
  if (ret != TTS_ERROR_NONE) {
    LOG(ERROR) << "TTS: Fail to add text";
    GetTtsMessageFilterEfl()->
      SpeakingErrorOccurred(utterance_.id, ErrorToString(static_cast<tts_error_e>(ret)));
    return;
  }
  tts_state_e current_state;
  tts_get_state(tts_handle_, &current_state);
  if (TTS_STATE_PLAYING != current_state)
    ret = tts_play(tts_handle_);

  if (ret != TTS_ERROR_NONE) {
    LOG(ERROR) << "TTS: Play Error occured";
    GetTtsMessageFilterEfl()->
      SpeakingErrorOccurred(utterance_.id, ErrorToString(static_cast<tts_error_e>(ret)));
    return;
  }
}

void TtsTizen::Pause() {
  if (!utterance_.id)
    return;

  int ret = tts_pause(tts_handle_);
  if (ret != TTS_ERROR_NONE) {
    LOG(ERROR) << "TTS: Fail to pause #tts_pause";
    GetTtsMessageFilterEfl()->
      SpeakingErrorOccurred(utterance_.id, ErrorToString(static_cast<tts_error_e>(ret)));
  }
}

void TtsTizen::Resume() {
  if (!utterance_.id)
    return;

  int ret = tts_play(tts_handle_);
  if (ret != TTS_ERROR_NONE) {
    LOG(ERROR) << "TTS: Fail to resume";
    GetTtsMessageFilterEfl()->
      SpeakingErrorOccurred(utterance_.id, ErrorToString(static_cast<tts_error_e>(ret)));
  }
}

void TtsTizen::Cancel() {
  int ret = tts_stop(tts_handle_);
  if (ret != TTS_ERROR_NONE) {
    LOG(ERROR) << "TTS: Fail to cancel";
    GetTtsMessageFilterEfl()->
      SpeakingErrorOccurred(utterance_.id, ErrorToString(static_cast<tts_error_e>(ret)));
  }
  GetTtsMessageFilterEfl()->DidFinishSpeaking(utterance_.id);
}

void TtsTizen::AddVoice(std::string language, tts_voice_type_e type) {
  TtsVoice voice;
  std::string uri(kUriPrefix);
  uri.append(VoiceTypeToName(type));
  uri.append("/");
  uri.append(language);

  voice.voice_uri = uri ;
  voice.name = VoiceTypeToName(type);
  voice.lang = language;
  voice.is_default = (language == default_language_);
  voice_list_.push_back(voice);
}

tts_speed_e TtsTizen::SetVoiceSpeed(float rate) {
  if (rate < 0.5)
    return TTS_SPEED_VERY_SLOW;
  else if (rate < 0.9)
    return TTS_SPEED_SLOW;
  else if (rate < 2)
    return TTS_SPEED_NORMAL;
  else if (rate < 5)
    return TTS_SPEED_FAST;
  else
    return TTS_SPEED_VERY_FAST;
}

}  // namespace content
