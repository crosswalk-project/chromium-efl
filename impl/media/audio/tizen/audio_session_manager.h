/*
 * Copyright (C) 2014 Samsung Electronics. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef AUDIO_SESSION_MANAGER_H_
#define AUDIO_SESSION_MANAGER_H_

#include <audio-session-manager.h>
#include <gst/gst.h>
#include <mm_session.h>
#include <mm_session_private.h>

#include "base/memory/ref_counted.h"

namespace media {

class AudioSessionManager : public base::RefCounted<AudioSessionManager> {
public:
  static scoped_refptr<AudioSessionManager> CreateAudioSessionManager() {
      return make_scoped_refptr(new AudioSessionManager());
  }

  static void SetVolumeSessionToMediaType();
  static void ClearVolumeSessionFromMediaType();

  virtual ~AudioSessionManager();

  bool RegisterAudioSessionManager(MMSessionType, ASM_sound_cb_t, void*);
  bool UnregisterAudioSessionManager();

  int GetSoundState(ASM_sound_states_t*);
  bool SetSoundState(ASM_sound_states_t);
  ASM_sound_events_t InterpretSessionToEvent(MMSessionType);
  bool AllocateResources(GstElement*);
  bool DeallocateResources();

  bool IsEmpty() const;

private:
  AudioSessionManager();

  ASM_sound_cb_t m_notifyCallback_;
  ASM_sound_events_t m_eventType_;
  ASM_sound_states_t m_stateType_;
  ASM_request_resources_t m_resources_;

  MMSessionType m_sessionType_;

  bool m_allocated_;
  int m_handle_;
  int m_processIdentifier_;
};
}
#endif
