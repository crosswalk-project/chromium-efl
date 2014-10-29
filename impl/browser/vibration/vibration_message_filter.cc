// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "browser/vibration/vibration_message_filter.h"

#include <algorithm>

#include "base/numerics/safe_conversions.h"
#include "base/memory/ref_counted.h"
#include "content/common/view_messages.h"
#include "content/public/browser/browser_thread.h"
#include "browser/vibration/vibration_provider_client.h"
#include "third_party/WebKit/public/platform/WebVibration.h"

// Minimum duration of a vibration is 1 millisecond.
const int64 kMinimumVibrationDurationMs = 1;

VibrationMessageFilter::VibrationMessageFilter()
    : BrowserMessageFilter(ViewMsgStart) {
}

void VibrationMessageFilter::OverrideThreadForMessage(
    const IPC::Message& message, content::BrowserThread::ID* thread) {
  switch (message.type()) {
    case ViewHostMsg_Vibrate::ID :
    case ViewHostMsg_CancelVibration::ID :
      *thread = content::BrowserThread::UI;
      break;
  }
}

bool VibrationMessageFilter::OnMessageReceived(const IPC::Message& message) {
  bool handled = true;
  IPC_BEGIN_MESSAGE_MAP(VibrationMessageFilter, message)
    IPC_MESSAGE_HANDLER(ViewHostMsg_Vibrate, OnVibrate)
    IPC_MESSAGE_HANDLER(ViewHostMsg_CancelVibration, OnCancelVibration)
    IPC_MESSAGE_UNHANDLED(handled = false)
  IPC_END_MESSAGE_MAP()
  return handled;
}

void VibrationMessageFilter::OnVibrate(int64 milliseconds) {
  // Though the Blink implementation already sanitizes vibration times, don't
  // trust any values passed from the renderer.
  milliseconds = std::max(kMinimumVibrationDurationMs,
      std::min(milliseconds,
          base::checked_cast<int64>(blink::kVibrationDurationMax)));
  VibrationProviderClient* vibration_provider = VibrationProviderClient::GetInstance();
  vibration_provider->Vibrate(milliseconds);
}

void VibrationMessageFilter::OnCancelVibration() {
  VibrationProviderClient* vibration_provider = VibrationProviderClient::GetInstance();
  vibration_provider->CancelVibration();
}
