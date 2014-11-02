// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MESSAGE_PUMP_FOR_UI_EFL
#define MESSAGE_PUMP_FOR_UI_EFL

#include "base/message_loop/message_pump.h"
#include "base/synchronization/lock.h"
#include <Ecore.h>
#include <Eina.h>

namespace base {

class RunLoop;
class TimeTicks;

class BASE_EXPORT MessagePumpForUIEfl : public base::MessagePump {
 public:
  MessagePumpForUIEfl();
  virtual ~MessagePumpForUIEfl();

  virtual void Run(Delegate *) override;
  virtual void Quit() override;
  virtual void ScheduleWork() override;
  virtual void ScheduleDelayedWork(const base::TimeTicks&) override;

 private:
  static void PipeCallback(void*, void*, unsigned int);
  static Eina_Bool TimerCallback(void*);
  void DoWork();
  void DoDelayedWork();

  Ecore_Pipe* pipe_;
  Delegate* delegate_;
  RunLoop* run_loop_;
  base::Lock schedule_work_lock_;
  bool work_scheduled_;

  DISALLOW_COPY_AND_ASSIGN(MessagePumpForUIEfl);
};

}

#endif
