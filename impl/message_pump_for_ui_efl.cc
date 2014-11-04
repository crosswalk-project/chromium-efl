// Copyright 2013 Samsung Electronics. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "message_pump_for_ui_efl.h"

#define private public
#include "base/run_loop.h"
#undef private
#include "base/time/time.h"
#include <Ecore.h>

namespace base {

namespace {
static const char dummy_pipe_message[] = "W";
static const int dummy_pipe_message_size = 1;
}

MessagePumpForUIEfl::MessagePumpForUIEfl()
  : pipe_(ecore_pipe_add(&PipeCallback, this))
  , delegate_(base::MessageLoopForUI::current())
  , run_loop_(new RunLoop())
  , work_scheduled_(false) {
  // Since the RunLoop was just created above, BeforeRun should be guaranteed to
  // return true (it only returns false if the RunLoop has been Quit already).
  bool result = run_loop_->BeforeRun();
  DCHECK(result);
}

MessagePumpForUIEfl::~MessagePumpForUIEfl() {
  for (std::unordered_set<TimerPair*>::iterator it = pending_timers_.begin();
       it != pending_timers_.end(); ++it) {
    ecore_timer_del((*it)->second);
    delete *it;
  }
}

// FIXME: need to be implemented for tests.
void MessagePumpForUIEfl::Run(base::MessagePump::Delegate* delegate) {
  NOTREACHED();
}

// FIXME: need to be implemented for tests.
void MessagePumpForUIEfl::Quit() {
  // RunLoop must be destroyed before chromium cleanup
  ecore_pipe_del(pipe_);
  DCHECK(run_loop_->running());
  run_loop_->AfterRun();
  delete run_loop_;
  run_loop_ = NULL;
  pipe_ = NULL;
}

void MessagePumpForUIEfl::ScheduleWork() {
  {
    base::AutoLock locker(schedule_work_lock_);
    if (work_scheduled_)
      return;
    work_scheduled_ = true;
  }

  DCHECK(pipe_);
  bool ok = ecore_pipe_write(pipe_, dummy_pipe_message, dummy_pipe_message_size);
  DCHECK(ok);
}

void MessagePumpForUIEfl::ScheduleDelayedWork(const base::TimeTicks& delayed_work_time) {
  base::TimeTicks now = base::TimeTicks::Now();
  double delay;
  if (delayed_work_time < now)
    delay = 0;
  else
    delay = base::TimeDelta(delayed_work_time - now).InSecondsF();

  TimerPair* new_pair = new TimerPair();
  new_pair->first = this;
  new_pair->second = ecore_timer_add(delay, &TimerCallback, new_pair);
  pending_timers_.insert(new_pair);
}

void MessagePumpForUIEfl::PipeCallback(void *data, void*, unsigned int) {
  static_cast<MessagePumpForUIEfl*>(data)->DoWork();
}

Eina_Bool MessagePumpForUIEfl::TimerCallback(void* data) {
  TimerPair* current_timer_pair = static_cast<TimerPair*>(data);
  current_timer_pair->first->DoDelayedWork();
  current_timer_pair->first->pending_timers_.erase(current_timer_pair);

  delete current_timer_pair;
  return false;
}

void MessagePumpForUIEfl::DoWork() {
  {
    base::AutoLock locker(schedule_work_lock_);
    DCHECK(work_scheduled_);
    work_scheduled_ = false;
  }

  bool more_work_is_plausible = delegate_->DoWork();

  base::TimeTicks delayed_work_time;
  more_work_is_plausible |= delegate_->DoDelayedWork(&delayed_work_time);

  if (more_work_is_plausible) {
    ScheduleWork();
    return;
  }

  more_work_is_plausible |= delegate_->DoIdleWork();
  if (more_work_is_plausible) {
    ScheduleWork();
    return;
  }

  if (!delayed_work_time.is_null())
    ScheduleDelayedWork(delayed_work_time);
}

void MessagePumpForUIEfl::DoDelayedWork() {
  base::TimeTicks next_delayed_work_time;
  delegate_->DoDelayedWork(&next_delayed_work_time);

  if (!next_delayed_work_time.is_null())
    ScheduleDelayedWork(next_delayed_work_time);
}

}
