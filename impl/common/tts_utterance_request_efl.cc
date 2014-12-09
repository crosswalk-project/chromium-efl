// Copyright (c) 2014 Samsung Electronics Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "tts_utterance_request_efl.h"

namespace content {

TtsUtteranceRequest::TtsUtteranceRequest()
    : id(0),
      volume(1.0),
      rate(1.0),
      pitch(1.0) {
}

TtsVoice::TtsVoice()
    : local_service(true),
      is_default(false) {
}

TtsUtteranceResponse::TtsUtteranceResponse()
    : id(0) {
}

}  // namespace content
