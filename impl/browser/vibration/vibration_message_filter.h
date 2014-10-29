// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VIBRATION_MESSAGE_FILTER_H_
#define VIBRATION_MESSAGE_FILTER_H_

#include "base/compiler_specific.h"
#include "content/public/browser/browser_message_filter.h"

class VibrationMessageFilter : public content::BrowserMessageFilter {
 public:
  explicit VibrationMessageFilter();

  // BrowserMessageFilter implementation.
  virtual void OverrideThreadForMessage(
      const IPC::Message& message,
      content::BrowserThread::ID* thread) OVERRIDE;
  virtual bool OnMessageReceived(const IPC::Message& message) OVERRIDE;

 private:
  virtual ~VibrationMessageFilter() {}

  void OnVibrate(int64 milliseconds);
  void OnCancelVibration();

  DISALLOW_COPY_AND_ASSIGN(VibrationMessageFilter);
};

#endif  // VIBRATION_MESSAGE_FILTER_H_
