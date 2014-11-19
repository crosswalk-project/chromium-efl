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

#include "media/audio/tizen/audio_session_manager.h"

#include <cstdlib>
#include <cstring>
#include <memory>
#include <unistd.h>

#include "base/logging.h"
#include "config.h"

namespace media {

AudioSessionManager::AudioSessionManager()
  : m_eventType_(ASM_EVENT_NONE)
  , m_handle_(-1)
  , m_notifyCallback_(0)
  , m_processIdentifier_(getpid())
  , m_sessionType_(MM_SESSION_TYPE_NUM)
  , m_stateType_(ASM_STATE_NONE)
  , m_allocated_(false)
  , m_resources_() {
}

AudioSessionManager::~AudioSessionManager() {
  if (m_allocated_)
    DeallocateResources();

  UnregisterAudioSessionManager();
}

bool AudioSessionManager::RegisterAudioSessionManager(
    MMSessionType sessiontype,
    ASM_sound_cb_t notifyCallback,
    void* callbackData) {
  int error = _mm_session_util_read_type
      (m_processIdentifier_, reinterpret_cast<int*>(&m_sessionType_));
  if (error) {
    m_sessionType_ = sessiontype;
    error = mm_session_init((int)m_sessionType_);
    if (error) {
      LOG(ERROR) << "mm_session_init() failed.";
      return false;
    }
  }
  m_eventType_ = InterpretSessionToEvent(m_sessionType_);
  m_notifyCallback_ = notifyCallback;
  if (!ASM_register_sound
        (m_processIdentifier_, &m_handle_, m_eventType_, m_stateType_,
         m_notifyCallback_, callbackData, ASM_RESOURCE_NONE, &error)) {
    LOG(ERROR) << "register is failed. errcode = " << error;
    return false;
  }
  return true;
}

bool AudioSessionManager::UnregisterAudioSessionManager() {
  int error = 0;
  if (!ASM_unregister_sound(m_handle_, m_eventType_, &error)) {
    LOG(ERROR) << "unregister is failed. errcode = " << error;
    return false;
  }
  return true;
}

int AudioSessionManager::GetSoundState(ASM_sound_states_t* state) {
  int error = 0;
  if (!ASM_get_sound_state( m_handle_, m_eventType_, state, &error))
    LOG(ERROR) << "getSoundState is failed. errcode = " << error;
  return error;
}

bool AudioSessionManager::SetSoundState(ASM_sound_states_t state) {
  int error = 0;
  if (!ASM_set_sound_state(
      m_handle_, m_eventType_, state, ASM_RESOURCE_NONE, &error)) {
    LOG(ERROR) << "setSoundState is failed. errcode = " << error;
    return false;
  }
  return true;
}

ASM_sound_events_t AudioSessionManager::InterpretSessionToEvent(
    MMSessionType sessionType) {
  switch (sessionType) {
  case MM_SESSION_TYPE_SHARE:
    return ASM_EVENT_SHARE_MMPLAYER;
  case MM_SESSION_TYPE_EXCLUSIVE:
    return ASM_EVENT_EXCLUSIVE_MMPLAYER;
  default:
    return ASM_EVENT_NONE;
  }
}

bool AudioSessionManager::AllocateResources(GstElement* element) {
  GList* list = NULL;
  gst_element_query_resource(element, &list);

  ASM_request_resources_t toAllocate;
  toAllocate.requested_resources_num = 0;
  int i = 0;
  for (GList* resource(list); resource; resource = resource->next) {
    toAllocate.device_category[toAllocate.requested_resources_num]
        = static_cast<ASM_resource_t>(GPOINTER_TO_INT(resource->data));
    toAllocate.access_right[toAllocate.requested_resources_num]
        = ASM_RESOURCE_ACCESS_RIGHT_EXCLUSIVE;

      ++toAllocate.requested_resources_num;
  }
  g_list_free(list);

  if (!toAllocate.requested_resources_num) {
    LOG(ERROR) << "No resources requested by : " << GST_ELEMENT_NAME(element);
    return true;
  }

  if (MAX_RESOURCE_NUM < toAllocate.requested_resources_num
        + m_resources_.requested_resources_num) {
    toAllocate.requested_resources_num = 0;
    return false;
  }

  int error(0);
  ASM_allocate_resources(m_handle_, &toAllocate, false, &error);
  if (error) {
    LOG(ERROR) << "Error allocating ASM resource. errcode : " << error;
    return false;
  }

  // Register this resources as allocated for future deallocation
  for (int i = 0; i < toAllocate.requested_resources_num; ++i) {
    m_resources_.device_id[m_resources_.requested_resources_num]
        = toAllocate.device_id[i];
    m_resources_.device_category[m_resources_.requested_resources_num]
        = toAllocate.device_category[i];
    m_resources_.access_right[m_resources_.requested_resources_num]
        = toAllocate.access_right[i];
    ++m_resources_.requested_resources_num;
  }
  m_allocated_ = true;
  return true;
}

bool AudioSessionManager::DeallocateResources() {
  // No good rollback from failure during resource deallocation.
  // The only smart move in this case would be to kill process,
  // but do we really want to do it?
  // Hence we ignore possible errors during resource deallocation.
  int error(0);
  m_allocated_ = !ASM_deallocate_resources(m_handle_, &m_resources_, &error);
  if (error)
    LOG(ERROR) << "Error deallocating ASM resources. errcode : " << error;
  m_resources_.requested_resources_num = 0;
  return !m_allocated_;
}

}
