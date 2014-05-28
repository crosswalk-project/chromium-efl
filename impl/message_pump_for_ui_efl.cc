/*
   Copyright (C) 2013 Samsung Electronics

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "message_pump_for_ui_efl.h"

#include "base/run_loop.h"
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
  DCHECK(run_loop_->running());
  run_loop_->AfterRun();
  delete run_loop_;
}

// FIXME: need to be implemented for tests.
void MessagePumpForUIEfl::Run(base::MessagePump::Delegate* delegate) {
  NOTREACHED();
}

// FIXME: need to be implemented for tests.
void MessagePumpForUIEfl::Quit() {
  NOTREACHED();
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

  Ecore_Timer* result = ecore_timer_add(delay, &TimerCallback, this);
  DCHECK(result);
}

void MessagePumpForUIEfl::PipeCallback(void *data, void*, unsigned int) {
  static_cast<MessagePumpForUIEfl*>(data)->DoWork();
}

Eina_Bool MessagePumpForUIEfl::TimerCallback(void* data) {
  static_cast<MessagePumpForUIEfl*>(data)->DoDelayedWork();
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
