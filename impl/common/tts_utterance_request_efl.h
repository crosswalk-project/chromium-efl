// Copyright (c) 2014 Samsung Electronics Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_COMMON_TTS_UTTERANCE_REQUEST_H_
#define CONTENT_COMMON_TTS_UTTERANCE_REQUEST_H_

#include "base/basictypes.h"
#include "base/strings/string16.h"

#include <vector>

namespace content {

struct TtsUtteranceRequest {
  TtsUtteranceRequest();

  int id;
  std::string text;
  std::string lang;
  std::string voice;
  float volume;
  float rate;
  float pitch;
};

struct TtsVoice {
  TtsVoice();

  std::string voice_uri;
  std::string name;
  std::string lang;
  bool local_service;
  bool is_default;
};

struct TtsUtteranceResponse {
  TtsUtteranceResponse();

  int id;
};

}  // namespace content

#endif  // CONTENT_COMMON_TTS_UTTERANCE_REQUEST_H_
