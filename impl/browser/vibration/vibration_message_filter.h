/*
 * Copyright (C) 2013 Samsung Electronics. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 */

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
