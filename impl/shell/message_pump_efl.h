// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Copyright (c) 2014 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_MESSAGE_LOOP_MESSAGE_PUMP_EFL_H_
#define BASE_MESSAGE_LOOP_MESSAGE_PUMP_EFL_H_

#include "base/base_export.h"
#include "base/basictypes.h"
#include "base/message_loop/message_pump.h"
#include "base/threading/non_thread_safe.h"

#include <Ecore.h>

namespace base {

class TimeTicks;

class BASE_EXPORT MessagePumpEfl : public MessagePump {
 public:
  MessagePumpEfl();
  virtual ~MessagePumpEfl();

  virtual void Run(Delegate* delegate) override;
  virtual void Quit() override;
  virtual void ScheduleWork() override;
  virtual void ScheduleDelayedWork(const TimeTicks& delayed_work_time) override;

  void HandleScheduledWork();

private:
  // We may make recursive calls to Run, so we save state that needs to be
  // separate between them in this structure type.
  struct RunState;
  RunState* state_;

  // This is the time when we need to do delayed work.
  TimeTicks delayed_work_time_;

  Ecore_Pipe* pipe_;
  base::Lock schedule_work_lock_;
  bool work_scheduled_;
};

}  // namespace base

#endif // BASE_MESSAGE_LOOP_MESSAGE_PUMP_EFL_H_
